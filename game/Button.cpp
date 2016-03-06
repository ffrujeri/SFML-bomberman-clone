#include "Button.hpp"
#include "Utility.hpp"
#include "SoundPlayer.hpp"
#include "ResourceHolder.hpp"
#include "Preferences.hpp"
#include "Application.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics.hpp>


namespace GUI
{

Button::Button(State::Context context)
: mCallback()
, mRectangle(sf::Vector2f(200,40), 5,50)
, mText("", context.fonts->get(Fonts::Main), 18)
, mIsToggle(false)
, mSounds(*context.sounds)
, mSprite()
, mEventHandler()
{
	changeTexture(Normal);

	sf::Vector2f bounds = mRectangle.getSize();
	mText.setPosition(bounds.x / 2.f, bounds.y / 2.f);
}

void Button::setCallback(Callback callback)
{
	mCallback = std::move(callback);
}

void Button::setText(const std::string& text)
{
	mText.setString(text);
	centerOrigin(mText);
}

void Button::setToggle(bool flag)
{
	mIsToggle = flag;
}

bool Button::isSelectable() const
{
    return true;
}

void Button::select()
{
	Component::select();

	changeTexture(isActive() ? Pressed : Hovered);
}

void Button::deselect()
{
	Component::deselect();

	changeTexture(Normal);
}

void Button::activate()
{
	Component::activate();

    // If we are toggle then we should show that the button is pressed and thus "toggled".
	if (mIsToggle)
		changeTexture(Pressed);

	if (mCallback)
		mCallback();

    // If we are not a toggle then deactivate the button since we are just momentarily activated.
	if (!mIsToggle)
		deactivate();

	if(Preferences::getAsBool("playSounds"))
		mSounds.play(SoundEffect::Button);
}

void Button::deactivate()
{
	Component::deactivate();

	if (mIsToggle)
	{
        // Reset texture to right one depending on if we are selected or not.
		if (isSelected())
			changeTexture(Hovered);
		else
			changeTexture(Normal);
	}
}

void Button::handleEvent(const sf::Event& event)
{
	if(mEventHandler) mEventHandler(event);
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();

	if (mDrawRect) target.draw(mRectangle, states);
	if (mDrawSprite) {
		target.draw(mSprite, states);
	}
	target.draw(mText, states);
}

void Button::changeTexture(Type buttonType)
{
	if (buttonType==Normal){
		mRectangle.setFillColor(mColorNormal);
		if(mDrawSprite && mTextureNormal!=NULL) mSprite.setTexture(*mTextureNormal);
	}
	else if (buttonType == Hovered){
		mRectangle.setFillColor(mColorHover);
		if(mDrawSprite && mTextureHover!=NULL) mSprite.setTexture(*mTextureHover);
	}
	else if (buttonType==Pressed){
		mRectangle.setFillColor(mColorPressed);
		if(mDrawSprite && mTextureHover!=NULL) mSprite.setTexture(*mTextureHover);
	}
}

bool Button::contains(int x, int y) {

	int mx = getPosition().x, my = getPosition().y;
	int xmin, ymin, width, height;

	if (mDrawRect) {
		xmin = mRectangle.getPosition().x;
		ymin = mRectangle.getPosition().y;
		width = mRectangle.getSize().x;
		height = mRectangle.getSize().y;
		if (x <= mx+xmin+width && x>=mx+xmin && y <= my+ymin+height && y>=my+ymin) return true;
	}
	if (mDrawSprite) {
		xmin = mSprite.getPosition().x;
		ymin = mSprite.getPosition().y;
		width = mSprite.getGlobalBounds().width;
		height = mSprite.getGlobalBounds().height;
		if (x <= mx+xmin+width && x>=mx+xmin && y <= my+ymin+height && y>=my+ymin) return true;
	}
	return false;
}

void Button::setHover(bool flag) {
	mIsToggle = flag;
}

void Button::setNormalColor(sf::Color color) {
	mColorNormal = color;
	if (!isSelected()) changeTexture(Type::Normal);
}

void Button::setHoverColor(sf::Color color) {
	mColorHover = color;
	if (isSelected()) changeTexture(Type::Hovered);
}

void Button::setSize(int width, int height) {
	if (mDrawRect) mRectangle.setSize(sf::Vector2f(width, height));
	mText.setPosition(width / 2.f, height / 2.f);
}

sf::Sprite* Button::getSprite() {
	return &mSprite;
}

void Button::drawSprite(bool flag) {
	mDrawSprite = flag;
}

void Button::drawRect(bool flag) {
	mDrawRect = flag;
}

void Button::setNormalTexture(sf::Texture* texture) {
	mTextureNormal = texture;
	if (!isSelected()) changeTexture(Type::Normal);
}

void Button::setHoverTexture(sf::Texture* texture) {
	mTextureHover = texture;
	if (isSelected()) changeTexture(Type::Hovered);
}

void Button::hoveredAt(int x, int y) {
	if (!isActive()) {
		if (contains(x,y)) {
			if (!mIsHovered) {
				changeTexture(Type::Hovered);
				mIsHovered = true;
			}
		}
		else if (mIsHovered) {
			changeTexture(Type::Normal);
			mIsHovered = false;
		}
	}
}

void Button::clickedAt(int x, int y) {
	if (contains(x, y)) activate();
}

Button::Button() : mCallback()
, mRectangle(sf::Vector2f(200, 40), 5, 50)
, mText("", Application::app->getFontHolder()->get(Fonts::Main), 18)
, mIsToggle(false)
, mSounds(*Application::app->getSoundPlayer())
,mSprite()
,mEventHandler()
{
	changeTexture(Normal);

	sf::Vector2f bounds = mRectangle.getSize();
	mText.setPosition(bounds.x / 2.f, bounds.y / 2.f);
}

void Button::setEventHandler(std::function<void(const sf::Event &)> eventHandler) {
	mEventHandler = std::move(eventHandler);
}

	bool Button::handleEventGently() {
		return !(mIsToggle&&isActive());
	}

	bool Button::captureMouseWhenActive() {
		return mIsToggle;
	}

	void Button::setPressedColor(sf::Color color) {
		mColorPressed = color;
		if (isActive()&& mIsToggle) changeTexture(Type::Pressed);
	}
}

