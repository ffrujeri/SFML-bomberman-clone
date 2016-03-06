//
// Created by Côme Weber on 05/02/2016.
//

#ifndef BOMBERMAN_NAVIGATIONBAR_HPP
#define BOMBERMAN_NAVIGATIONBAR_HPP

#include "Component.hpp"
#include "RoundedRectangleShape.hpp"
#include <SFML/Graphics.hpp>

namespace GUI {
    class NavigationBar : public Component {

    private:

        std::vector<std::string> mCells;
        std::vector<float> mCellsLength;
        sf::Color mNormalColor = sf::Color(31, 188, 153);
        sf::Color mHoverColor = sf::Color(74, 201, 174);
        sf::Color mSelectedColor = sf::Color(26, 160, 131);
        sf::Color mTextColor = sf::Color::White;
        sf::Font  mFont = sf::Font();

        int mCurrentTab=0;
        int mHoveredTab = -1;
        int mCellMargin = 10;
        //number of pixel margin on the border of each cell
        int mHeight = 30;
        int mRadius = 5; //rounded rectangle corner radius



    public:


        NavigationBar();

        NavigationBar(sf::Font &font);

        virtual bool isSelectable() const;

        virtual void handleEvent(const sf::Event &event);

        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

        int getCurrentTab() const;

        void addCell(std::string cellName);

        void selectNextTab();

        void selectPreviousTab();

        void selectTab(int i);

        virtual sf::Vector2f getSize() override;

        virtual void clickedAt(int x, int y) override;

        virtual void hoveredAt(int x, int y) override;

        virtual bool contains(int x, int y) override;

    protected:

        /**
        * returns the cell number at position (x,y)
        * returns -1 if out of bounds
        */
        virtual int getHoveredTab(int x, int y);
    };

}
#endif //BOMBERMAN_NAVIGATIONBAR_HPP
