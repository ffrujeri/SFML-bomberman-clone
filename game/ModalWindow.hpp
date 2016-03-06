//
// Created by Côme Weber on 04/02/2016.
//

#ifndef BOMBERMAN_MODALWINDOW_HPP
#define BOMBERMAN_MODALWINDOW_HPP



#include <SFML/Graphics.hpp>
#include "Component.hpp"
#include "Container.hpp"
#include "RoundedRectangleShape.hpp"

namespace GUI {

    class ModalWindow : public Component {

    protected:


        Container mModal;

        sf::Color mDarkBackgroundColor = sf::Color(0, 0, 0, 170);

        int mWidth, mHeight, mBackWidth, mBackHeight;

        RoundedRectangleShape mModalBackground;

        bool mIsActive = false;

        std::function<void()> mDeactivationCallback = [](){};

    public:


        ModalWindow(int backWidth, int backHeight, int width, int height) :
                mModal(),
                mModalBackground(sf::Vector2f(width, height), 15, 10)
                {

            mWidth = width;
            mHeight = height;
            mBackHeight = backHeight;
            mBackWidth = backWidth;

            mModal.setPosition((backWidth-width)/2,(backHeight-height)/2);
            mModalBackground.setPosition((backWidth-width)/2,(backHeight-height)/2);
            mModalBackground.setFillColor(sf::Color::White);
        }


        virtual bool isActive() const override;

        virtual void activate() override;

        virtual void deactivate() override;

        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

        virtual bool isSelectable() const;

        virtual void handleEvent(const sf::Event &event);

        virtual void pack(Component::Ptr);

        virtual bool draw() override;

        virtual void setDeactivationCallback(std::function<void()> callback);


        virtual bool captureMouseWhenActive() override;

        virtual bool update(sf::Time dt) override;

    private:

        virtual bool inModal(int x,int y); //test wheter the point at coordinates (x,y) is inside the modal (ie not in the background zone)
    };
}


#endif //BOMBERMAN_MODALWINDOW_HPP
