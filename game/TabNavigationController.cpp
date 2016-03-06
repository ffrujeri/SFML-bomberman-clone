//
// Created by Côme Weber on 05/02/2016.
//

#include <SFML/Graphics.hpp>

#include "TabNavigationController.hpp"
#include "ResourceHolder.hpp"
#include "Utility.hpp"


namespace GUI {

    bool GUI::TabNavigationController::isSelectable() const {
        return true;
    }

    void GUI::TabNavigationController::handleEvent(const sf::Event &event) {



        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab) {
            mNavBar.selectNextTab();
        }
        else if (event.type == sf::Event::MouseButtonPressed) {
            mNavBar.clickedAt(event.mouseButton.x, event.mouseButton.y);
        }
        else if (event.type == sf::Event::MouseMoved) {
            mNavBar.hoveredAt(event.mouseMove.x, event.mouseMove.y);
        }

        mContainers[mNavBar.getCurrentTab()]->handleEvent(transformEvent(event,getPosition()));
    }

    void GUI::TabNavigationController::draw(sf::RenderTarget &target, sf::RenderStates states) const {

        states.transform *= getTransform();

        target.draw(mNavBar, states);

        target.draw(*mContainers[mNavBar.getCurrentTab()], states);

    }

    void GUI::TabNavigationController::clickedAt(int x, int y) {
        Component::clickedAt(x, y);
        mNavBar.clickedAt(x, y);
    }

    GUI::Container::Ptr GUI::TabNavigationController::addTab(std::string tabName) {
        const std::shared_ptr<Container> &container = std::make_shared<GUI::Container>();
        mContainers.push_back(container);
        mNavBar.addCell(tabName);
        mNavBar.setPosition((mWidth - mNavBar.getSize().x)/2., 2*mSeparatorDistance);
        container->setPosition(0, mNavBar.getPosition().y + mNavBar.getSize().y + 4*mSeparatorDistance);
        return container;
    }

    GUI::TabNavigationController::TabNavigationController(int width, int height)
            : mWidth(width),
              mHeight(height),
              mNavBar(),
              mContainers() {
    }

    bool TabNavigationController::update(sf::Time dt) {
        mNavBar.update(dt);
        return mContainers[mNavBar.getCurrentTab()]->update(dt);
    }
}