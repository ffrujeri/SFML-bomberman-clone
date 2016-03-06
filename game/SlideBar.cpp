//
// Created by Côme Weber on 04/02/2016.
//

#include <iostream>
#include "SlideBar.hpp"

bool GUI::SlideBar::isSelectable() const {
    return false;
}

void GUI::SlideBar::handleEvent(const sf::Event &event) {
    if (event.type==sf::Event::MouseButtonPressed){
        if (contains(event.mouseButton.x, event.mouseButton.y)){
            mouseAt(event.mouseButton.x, event.mouseButton.y);
            mMouseClickHolded = true;
        }
    }
    else if (mMouseClickHolded && event.type==sf::Event::MouseMoved){
            mouseAt(event.mouseMove.x, event.mouseMove.y);
    }
    else if (mMouseClickHolded && event.type==sf::Event::MouseButtonReleased){
        mMouseClickHolded = false;
    }
}

void GUI::SlideBar::draw(sf::RenderTarget &target, sf::RenderStates states) const {

    states.transform *= getTransform();

    RoundedRectangleShape rect(sf::Vector2f(mLength,mHeight),mHeight/2, 30);
    rect.setFillColor(sf::Color(230,230,230));
    target.draw(rect, states);
    sf::CircleShape bullet(mHeight/4);
    bullet.setFillColor(sf::Color(210,210,210));
    bullet.setOrigin(bullet.getRadius(),bullet.getRadius());
    if (mShowMarks) {
        for (int i=mCurrentStep+1; i<mSteps-1; i++){
            bullet.setPosition(mHeight/2+i*(mLength-mHeight)/((float)mSteps-1), mHeight/2);
            target.draw(bullet,states);
        }
    }
    rect.setFillColor(sf::Color(31,188,153));
    rect.setSize(sf::Vector2f(mHeight/2+(mLength-mHeight)*mCurrentStep/((float)mSteps-1),mHeight));
    target.draw(rect,states);
    bullet.setFillColor(sf::Color(26,160,131));
    bullet.setRadius(mHeight*0.75);
    bullet.setOrigin(bullet.getRadius(),bullet.getRadius());
    bullet.setPosition(mHeight/2+(mLength-mHeight)*mCurrentStep/((float)mSteps-1),mHeight/2);
    target.draw(bullet,states);
}

GUI::SlideBar::SlideBar(int numberOfSteps, int length) :
mSteps(numberOfSteps),
mLength(length),
mCircle(),
mRect(){

    mCurrentStep = numberOfSteps/2;

}

void GUI::SlideBar::mouseAt(int x, int y) {
    if ((!mMouseClickHolded && !contains(x,y)) || (mMouseClickHolded && !contains(x,getPosition().y+mHeight/2.f))) return;
    float division = (x-getPosition().x)/(mLength/((float) mSteps-1));
    mCurrentStep = (division - (int) division > 0.5 ? (int) division + 1 : (int) division);
    mCallback(mCurrentStep);
}

bool GUI::SlideBar::contains(int x, int y) {

    return x>=getPosition().x && x<=getPosition().x+mLength && y>=getPosition().y && y<=getPosition().y+mHeight;
}

void GUI::SlideBar::showMarks(bool flag) {
    mShowMarks=flag;
}

void GUI::SlideBar::setCallback(std::function<void(int)> callback) {
    mCallback = std::move(callback);
}

void GUI::SlideBar::setCursorPosition(int position) {
    if(position>=0 && position<mSteps) mCurrentStep = position;
}

bool GUI::SlideBar::handleEventGently() {
    return true;
}
