//
// Created by Côme Weber on 09/02/2016.
//

#include <cmath>
#include <iostream>
#include "CollectionView.hpp"
#include "Preferences.hpp"

namespace GUI {

    bool GUI::CollectionView::isSelectable() const {
        return false;
    }

    void GUI::CollectionView::handleEvent(const sf::Event &event) {

    }

    void CollectionView::draw(sf::RenderTarget &target, sf::RenderStates states) const {

        states.transform *= getTransform();

        for (auto button : mTiles) {
            target.draw(*button, states);
        }
    }

    void GUI::CollectionView::hoveredAt(int x, int y) {
        for (auto button : mTiles) {
           button->hoveredAt(x-getPosition().x, y-getPosition().y);
        }
    }

    void GUI::CollectionView::clickedAt(int x, int y) {
        for (int i=0; i<mTiles.size(); i++) {
            if (mTiles[i]->contains(x - getPosition().x, y - getPosition().y)){
                if(!mTiles[i]->isActive()){
                    mTiles[i]->activate();
                    mTiles[mSelectedTile]->deactivate();
                    mSelectedTile = i;
                    Preferences::addAsInt("PlayerSkin", mSelectedTile+1);
                }
            }
        }
    }

    sf::Vector2f GUI::CollectionView::getSize() {
        return sf::Vector2f(mWidth, mHeight);
    }

    void CollectionView::setSize(sf::Vector2f size) {
        mWidth = size.x;
        mHeight = size.y;
        updateGraphics();
    }

    void CollectionView::setTileSize(sf::Vector2f size) {
        mTileWidth = size.x;
        mTileHeight = size.y;
        updateGraphics();
    }

    void CollectionView::addTile(std::string imgPath, sf::Vector2f textureOffset, sf::Vector2f textureSize) {
        auto tile = std::make_shared<GUI::Button>();
        tile->setText("");
        tile->drawRect(true);
        tile->drawSprite(true);
        sf::Texture* texture = new sf::Texture();
        if (textureSize.x != -1){
            texture->loadFromFile(imgPath,sf::IntRect((int) textureOffset.x, (int) textureOffset.y,(int)textureSize.x, (int)textureSize.y));
        }
        else texture->loadFromFile(imgPath);
        tile->setNormalTexture(texture);
        tile->setPressedColor(sf::Color(75,131,204));
        tile->setToggle(true);
        mTiles.push_back(tile);
        updateGraphics();
    }

    void CollectionView::updateGraphics() {
        int numbertileperline = std::floor((mWidth)/(mTileWidth+mSep));
        int border = (mWidth-std::min(numbertileperline,(int) mTiles.size())*(mTileWidth+mSep)+mSep)/2;
        for( int k=0;k<mTiles.size();k++){
            int j = k%numbertileperline;
            int i = k/numbertileperline;
            int x_position = border + j*(mTileWidth+mSep);
            int y_position = mSep + i*(mTileHeight+mSep);
            mTiles[k]->setSize(mTileWidth,mTileHeight);
            mTiles[k]->setPosition(x_position,y_position);
            auto textSize = mTiles[k]->getSprite()->getTexture()->getSize();
            mTiles[k]->getSprite()->setPosition((mTileWidth - (int)textSize.x)/2.f,(mTileHeight - (int)textSize.y)/2.f);
        }
    }

    void CollectionView::addTile(std::string imgPath) {
        addTile(imgPath, sf::Vector2f(0, 0), sf::Vector2f(-1,-1));
    }

    void CollectionView::selectTile(int nbTile) {
        if (nbTile>0 && nbTile<=mTiles.size() && nbTile-1 != mSelectedTile){
            if (mSelectedTile!= -1) mTiles[mSelectedTile]->deactivate();
            mTiles[nbTile-1]->activate();
            mSelectedTile = nbTile -1;
        }
    }
}