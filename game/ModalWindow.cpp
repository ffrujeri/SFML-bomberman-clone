//
// Created by Côme Weber on 04/02/2016.
//

#include "ModalWindow.hpp"
#include "RoundedRectangleShape.hpp"
#include "Utility.hpp"

namespace GUI {

    void GUI::ModalWindow::draw(sf::RenderTarget &target, sf::RenderStates states) const {
        sf::RectangleShape background(sf::Vector2f(mBackWidth,mBackHeight));
        background.setFillColor(mDarkBackgroundColor);
        background.setPosition(0,0);
        target.draw(background,states);
        target.draw(mModalBackground,states);
        target.draw(mModal,states);
    }

    bool ModalWindow::isActive() const {
        return mIsActive;
    }

    void ModalWindow::activate() {
        mIsActive = true;
    }

    void ModalWindow::deactivate() {
        mIsActive = false;
        mDeactivationCallback();
    }

    bool ModalWindow::isSelectable() const {
        return true;
    }

    void ModalWindow::handleEvent(const sf::Event &event) {


        sf::Event evt = transformEvent(event, getPosition());

        mModal.handleEvent(evt);

        if (evt.type == sf::Event::KeyPressed){
            if (event.key.code == sf::Keyboard::Escape){
                deactivate();
            }
        }
        
        if (evt.type == sf::Event::MouseButtonPressed){
            if(!inModal(evt.mouseButton.x, evt.mouseButton.y)) deactivate();
        }
    }

    bool ModalWindow::draw() {
        return isActive();
    }

    void ModalWindow::pack(Component::Ptr ptr) {
        mModal.pack(ptr);
    }

    bool ModalWindow::inModal(int x, int y) {
        return x>=(mBackWidth-mWidth)/2.f && x<=(mBackWidth+mWidth)/2.f && y>=(mBackHeight-mHeight)/2.f && y<=(mBackHeight+mHeight)/2.f;
    }

    void ModalWindow::setDeactivationCallback(std::function<void()> callback) {
        mDeactivationCallback = std::move(callback);
    }

    bool ModalWindow::update(sf::Time dt) {
        return mModal.update(dt);
    }

    bool ModalWindow::captureMouseWhenActive() {
        return true;
    }
}