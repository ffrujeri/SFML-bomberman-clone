//
// Created by Côme Weber on 06/02/2016.
//

#ifndef BOMBERMAN_CHECKBOX_HPP
#define BOMBERMAN_CHECKBOX_HPP

#include <functional>
#include "Component.hpp"
#include <SFML/Graphics.hpp>

namespace GUI {

    class Checkbox : public Component {

    private:
        bool isChecked = true;
        bool isHovered = false;

        std::function<void(bool)> mCallback = [](bool b){};

        sf::Color mUncheckedColor = sf::Color(188, 193, 197);
        sf::Color mCheckedColor = sf::Color(31, 188, 153);
        sf::Color mMarkColor = sf::Color::White;


        int mWidth=20, mHeight=mWidth;
        int mRadius = 5;



    public:


        Checkbox(bool isChecked) : isChecked(isChecked) { }

        Checkbox() { }

        virtual bool isSelectable() const;

        virtual void handleEvent(const sf::Event &event);

        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

        virtual bool contains(int x, int y) override;

        virtual void hoveredAt(int x, int y) override;

        virtual void clickedAt(int x, int y) override;

        virtual sf::Vector2f getSize() override;

        virtual void setCallback(std::function<void(bool)> callback);
    };

}
#endif //BOMBERMAN_CHECKBOX_HPP
