//
// Created by Côme Weber on 05/02/2016.
//

#include "NavigationBar.hpp"
#include "Application.hpp"

bool GUI::NavigationBar::isSelectable() const {
    return false;
}

void GUI::NavigationBar::handleEvent(const sf::Event &event) {

}

void GUI::NavigationBar::draw(sf::RenderTarget &target, sf::RenderStates states) const{

    states.transform *= getTransform();

    //first draw first and last cell
    RoundedRectangleShape roundedRectangle(sf::Vector2f(0,0),mRadius,20);
    sf::RectangleShape rectangle(sf::Vector2f(mRadius,mHeight));
    sf::Text text("",mFont,15);
    text.setColor(mTextColor);

    float sum = 0;
    for (int i=0; i<mCells.size()-1;i++) sum+= mCellsLength[i];

    for(int i =0; i<mCells.size(); i+=mCells.size()-1){
        roundedRectangle.setSize(sf::Vector2f(mCellsLength[i],mHeight));
        if (i==mCurrentTab) {
            rectangle.setFillColor(mSelectedColor);
            roundedRectangle.setFillColor(mSelectedColor);
        }
        else if(i==mHoveredTab) {
            rectangle.setFillColor(mHoverColor);
            roundedRectangle.setFillColor(mHoverColor);
        }
        else {
            rectangle.setFillColor(mNormalColor);
            roundedRectangle.setFillColor(mNormalColor);
        }
        roundedRectangle.setPosition((i==0 ?0:sum), 0);
        rectangle.setPosition((i>0?sum:mCellsLength[0]-mRadius), 0);
        target.draw(roundedRectangle, states);
        target.draw(rectangle,states);
        text.setString(mCells[i]);
        text.setPosition((i==0?0:sum) + mCellMargin, (mHeight - text.getLocalBounds().height - text.getLocalBounds().top) / 2);
        target.draw(text, states);
    }
    sum = mCellsLength[0];
    for(int i=1; i<mCells.size()-1; i++){
        rectangle.setSize(sf::Vector2f(mCellsLength[i],mHeight));
        if (i==mCurrentTab) rectangle.setFillColor(mSelectedColor);
        else if(i==mHoveredTab) rectangle.setFillColor(mHoverColor);
        else rectangle.setFillColor(mNormalColor);
        rectangle.setPosition(sum, 0);
        target.draw(rectangle, states);
        text.setString(mCells[i]);
        text.setPosition(sum + mCellMargin, (mHeight - text.getLocalBounds().height - text.getLocalBounds().top) / 2.f);
        target.draw(text, states);
        sum+=mCellsLength[i];
    }
}

void GUI::NavigationBar::addCell(std::string cellName) {
    mCells.push_back(cellName);
    sf::Text text(cellName,mFont,15);
    mCellsLength.push_back(text.getLocalBounds().width + 2 * mCellMargin);
}

int GUI::NavigationBar::getCurrentTab() const {
    return mCurrentTab;
}

GUI::NavigationBar::NavigationBar() : mCells(), mCellsLength(), mFont(Application::app->getFontHolder()->get(Fonts::Main)){
}

void GUI::NavigationBar::selectNextTab() {
    mCurrentTab = (mCurrentTab + 1) % mCells.size();
}

void GUI::NavigationBar::selectPreviousTab() {
    mCurrentTab = (mCurrentTab - 1) % mCells.size();
}

void GUI::NavigationBar::selectTab(int i) {
    mCurrentTab = i % mCells.size();
}

void GUI::NavigationBar::hoveredAt(int x, int y) {
    mHoveredTab = getHoveredTab(x,y);
}

void GUI::NavigationBar::clickedAt(int x, int y) {
    int tmp = getHoveredTab(x,y);
    if (tmp!=-1) mCurrentTab = tmp;
}

sf::Vector2f GUI::NavigationBar::getSize() {
    float sum = 0;
    for (float n : mCellsLength) sum+=n;
    return sf::Vector2f(sum, mHeight);
}

bool GUI::NavigationBar::contains(int x, int y) {
    return (x>=getPosition().x && x<=getPosition().x+getSize().x && y>=getPosition().y && y<=getPosition().y+getSize().y );
}

int GUI::NavigationBar::getHoveredTab(int x, int y) {
    if (!contains(x,y)) return -1;
    float sum = getPosition().x;
    int i=0;
    while(x>sum && i<mCellsLength.size())
        sum+=mCellsLength[i++];
    return i-1;
}

GUI::NavigationBar::NavigationBar(sf::Font &font) : mFont(font){

}
