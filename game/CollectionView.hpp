//
// Created by Côme Weber on 09/02/2016.
//

#ifndef BOMBERMAN_COLLECTIONVIEW_HPP
#define BOMBERMAN_COLLECTIONVIEW_HPP

#include "Component.hpp"
#include "Button.hpp"
#include <SFML/Graphics.hpp>

namespace GUI {

    /**
     * Component drawing a list of thumbnails of images.
     * Thumbnails can be selected individually
     */

    class CollectionView : public Component {

    protected:

        std::vector<std::shared_ptr<Button>> mTiles;
        int                 mWidth, mHeight;
        int                 mTileWidth, mTileHeight;
        int                 mSep = 20;//horizontal and vertical distance between tiles
        int                 mSelectedTile = -1;

    public:


        virtual bool isSelectable() const;

        virtual void handleEvent(const sf::Event &event);

        virtual void hoveredAt(int x, int y) override;

        virtual void clickedAt(int x, int y) override;

        virtual sf::Vector2f getSize() override;

        //select ith tile with i>=1 && i<=number of tiles
        virtual void selectTile(int nbTile);

        virtual void setSize(sf::Vector2f size);

        virtual void setTileSize(sf::Vector2f size);

        virtual void addTile(std::string imgPath);

        virtual void addTile(std::string imgPath, sf::Vector2f textureOffset, sf::Vector2f textureSize);

        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

        void updateGraphics();
    };
}

#endif //BOMBERMAN_COLLECTIONVIEW_HPP
