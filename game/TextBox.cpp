//
// Created by Côme Weber on 09/02/2016.
//

#include <iostream>
#include <math.h>
#include "TextBox.hpp"
#include "Utility.hpp"
#include "Application.hpp"

bool GUI::TextBox::isSelectable() const {
    return true;
}

void GUI::TextBox::handleEvent(const sf::Event &event) {
    //TODO handle keyboard and mouse
    if (event.type == sf::Event::KeyPressed){
        handleKey(event.key.code);
        mCurrentKey = event.key.code;
        mCursorBlinkCount = 0;
    }
    else if (event.type == sf::Event::TextEntered  && isUnicodeAllowed(event.text.unicode)) {
        mString = mString.insert(mCurrentCaretPosition, 1, static_cast<char>(event.text.unicode));
        mCurrentCaretPosition++;
        updateDrawConstants();
    }
    else if (event.type == sf::Event::MouseButtonPressed) {
        if (!contains(event.mouseButton.x, event.mouseButton.y)) {
            deactivate();
        }
    }
    else if (event.type==sf::Event::KeyReleased) {
        mCurrentKey=sf::Keyboard::Key::Unknown;
        mCurrentKeyCount=-10;
    }
}

void GUI::TextBox::handleKey(sf::Keyboard::Key key) {
    switch(key){
            case sf::Keyboard::Right:
                this->mCurrentCaretPosition = std::min(this->mCurrentCaretPosition + 1, (int) this->mString.size());
                break;
            case sf::Keyboard::Left:
                this->mCurrentCaretPosition = std::max(this->mCurrentCaretPosition - 1, 0);
                break;
            case sf::Keyboard::Escape:
                GUI::TextBox::deactivate();
                break;
            case sf::Keyboard::BackSpace:
                if (this->mCurrentCaretPosition > 0){
                    this->mString = this->mString.substr(0,mCurrentCaretPosition-1) + this->mString.substr(mCurrentCaretPosition);
                    this->mCurrentCaretPosition--;
                }
                break;
            case sf::Keyboard::Delete:
                if (this->mCurrentCaretPosition < mString.size()){
                    this->mString = this->mString.substr(0,mCurrentCaretPosition) + this->mString.substr(mCurrentCaretPosition+1);
                }
                break;
            default:
                mCurrentKey= sf::Keyboard::Unknown;
        }
    updateDrawConstants();
}

void GUI::TextBox::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    states.transform *= getTransform();

    sf::Text text(mString.compare("") == 0 ? mKeyHolder : mString, Application::app->getFontHolder()->get(Fonts::Main),
                  18);
    text.setColor(mString.compare("") == 0 ? mInactiveColor : mTextColor);
    text.setPosition(mViewOffset,mBackgroundRectangle.getSize().y*0.33f - (text.getLocalBounds().height/2.f+text.getLocalBounds().top));

    sf::RenderTexture renderTexture;
    renderTexture.create(mBackgroundRectangle.getSize().x-20,mBackgroundRectangle.getSize().y*0.66f);
    renderTexture.clear(sf::Color::White);
    renderTexture.draw(text); // or any other drawable


    if (isActive()) {

        if (mCursorBlinkCount<31) {
            sf::Text text2(mString.substr(0,mCurrentCaretPosition),Application::app->getFontHolder()->get(Fonts::Main),18);
            sf::RectangleShape rect(sf::Vector2f(2, mBackgroundRectangle.getSize().y*0.66f));
            rect.setFillColor(mActiveColor);
            rect.setPosition(text2.getGlobalBounds().width-1+mViewOffset,0);
            renderTexture.draw(rect);
        }
    }

    renderTexture.display();
    sf::Sprite sprite(renderTexture.getTexture());
    sprite.setPosition(10,mBackgroundRectangle.getSize().y/6.f);

    target.draw(mBackgroundRectangle, states);
    target.draw(sprite, states);
}

GUI::TextBox::TextBox(std::string keyHolder) :
        mBackgroundRectangle(sf::Vector2f(mWidth, mHeight), 5, 20) {
    mKeyHolder = keyHolder;
    mBackgroundRectangle.setOutlineThickness(2);
    mBackgroundRectangle.setOutlineColor(mInactiveColor);
}

void GUI::TextBox::setKeyHolder(std::string keyholder) {
    mKeyHolder = keyholder;
}

void GUI::TextBox::setSize(sf::Vector2f size) {
    mWidth = size.x;
    mHeight = size.y;
    mBackgroundRectangle.setSize(getSize());
}

void GUI::TextBox::setBackgroundColor(sf::Color color) {
    mBackgroundRectangle.setFillColor(color);
}

void GUI::TextBox::setTextColor(sf::Color color) {
    mTextColor = color;
}

void GUI::TextBox::setKeyHolderColor(sf::Color color) {
    mInactiveColor = color;
}

void GUI::TextBox::setText(std::string text) {
    mString = text;
    mCurrentCaretPosition = text.size();
}

sf::Vector2f GUI::TextBox::getSize() {
    return sf::Vector2f(mWidth, mHeight);
}

bool GUI::TextBox::contains(int x, int y) {
    return x >= getPosition().x && x <= getPosition().x + mWidth && y >= getPosition().y &&
           y <= getPosition().y + mHeight;
}

void GUI::TextBox::clickedAt(int x, int y) {
    if (contains(x, y)) activate();
}

void GUI::TextBox::activate() {
    Component::activate();
    mBackgroundRectangle.setOutlineColor(mActiveColor);
}

void GUI::TextBox::deactivate() {
    Component::deactivate();
    mBackgroundRectangle.setOutlineColor(mInactiveColor);
    mDeactivationCallback();
}

bool GUI::TextBox::update(sf::Time dt) {

    if (isActive()) {
        if (mCurrentKey != sf::Keyboard::Key::Unknown) {
            if (mCurrentKeyCount == 4) {
                handleKey(mCurrentKey);
                mCurrentKeyCount = 0;
                mCursorBlinkCount = 0;
            }
            mCurrentKeyCount++;
        }
        mCursorBlinkCount++;
        if (mCursorBlinkCount == 60) mCursorBlinkCount = 0;
    }
}

void GUI::TextBox::updateDrawConstants(){

    sf::Text text(mString.substr(0,mCurrentCaretPosition),Application::app->getFontHolder()->get(Fonts::Main),18);
    if (text.getGlobalBounds().width+mViewOffset<0){
        mViewOffset = -text.getGlobalBounds().width+1;
    }
    else if (text.getGlobalBounds().width+mViewOffset>mBackgroundRectangle.getSize().x-20){
        mViewOffset = mBackgroundRectangle.getSize().x-20-text.getGlobalBounds().width-2;
    }
}

bool GUI::TextBox::isUnicodeAllowed(sf::Uint32 unicode) {
    if (unicode == 8) return false;
    if (unicode == 10) return false;
    if (unicode == 9) return false;
    if (unicode==127) return false;
    return true;
}

bool GUI::TextBox::captureMouseWhenActive() {
    return false;
}

void GUI::TextBox::setDeactivationCallback(std::function<void()> callback) {
    mDeactivationCallback = std::move(callback);
}

std::string GUI::TextBox::getText() {
    return mString;
}
