//
// Created by Côme Weber on 04/02/2016.
//

#ifndef BOMBERMAN_SLIDEBAR_HPP
#define BOMBERMAN_SLIDEBAR_HPP

#include <SFML/Graphics.hpp>
#include "Component.hpp"
#include "RoundedRectangleShape.hpp"

namespace GUI {

class SlideBar : public Component {


private:
    RoundedRectangleShape mRect;
    sf::CircleShape       mCircle;
    int                   mLength;
    int                   mHeight=15;
    int                   mSteps;
    int                   mCurrentStep;
    bool                  mMouseClickHolded = false;
    bool                  mShowMarks = true;
    std::function<void(int)> mCallback = [](int){};

public:


    SlideBar(int numberOfSteps, int length);

    virtual bool isSelectable() const;

    virtual void handleEvent(const sf::Event &event);

    virtual void mouseAt(int x, int y);

    virtual bool contains(int x, int y) override;


    virtual bool handleEventGently() override;

    virtual void showMarks(bool flag);

    virtual void setCallback(std::function<void(int)> callback);
    /**
     * position must be within the range [0,nbSteps-1];
     */
    virtual void setCursorPosition(int position);

protected:
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;


};

}


#endif //BOMBERMAN_SLIDEBAR_HPP
