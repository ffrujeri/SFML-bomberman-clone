//
// Created by Côme Weber on 06/02/2016.
//

#include "Checkbox.hpp"
#include "RoundedRectangleShape.hpp"

bool GUI::Checkbox::isSelectable() const {
    return false;
}

void GUI::Checkbox::handleEvent(const sf::Event &event) {

}

void GUI::Checkbox::draw(sf::RenderTarget &target, sf::RenderStates states) const {

    states.transform *= getTransform();

    RoundedRectangleShape rect(sf::Vector2f(mWidth,mHeight), mRadius, 10);

    rect.setFillColor(isChecked ? mCheckedColor : mUncheckedColor);

    target.draw(rect, states);

    //draw the checkmark
    if (isChecked || isHovered){

        rect.setCornersRadius(1);
        rect.setFillColor(mMarkColor);
        states.transform.translate(mWidth/3.f,mHeight*0.7f).rotate(-45);
        rect.setPosition(0,-mWidth/4.f);
        rect.setSize(sf::Vector2f(2, mWidth/4.f));
        target.draw(rect, states);
        rect.setPosition(0,0);
        rect.setSize(sf::Vector2f(mWidth*.6f,2));
        target.draw(rect, states);
        states.transform.rotate(45).translate(-mWidth/2.f,-mHeight*0.8f);
    }

}

bool GUI::Checkbox::contains(int x, int y) {
    return x>=getPosition().x&& x<=getPosition().x + mWidth && y>=getPosition().y&& y<=getPosition().y + mHeight;
}

void GUI::Checkbox::hoveredAt(int x, int y) {
    isHovered = contains(x,y);
}

void GUI::Checkbox::clickedAt(int x, int y) {
    if (contains(x,y)){
        isChecked = !isChecked;
        mCallback(isChecked);
    }
}

sf::Vector2f GUI::Checkbox::getSize() {
    return sf::Vector2f(mWidth, mHeight);
}

void GUI::Checkbox::setCallback(std::function<void(bool)> callback) {
    mCallback = std::move(callback);
}
