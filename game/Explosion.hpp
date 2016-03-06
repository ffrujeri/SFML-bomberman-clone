//
// Created by Bruno Vegreville on 10/02/2016.
//

#ifndef BOMBERMAN_EXPLOSION_HPP
#define BOMBERMAN_EXPLOSION_HPP


#include "Entity.hpp"
#include "Command.hpp"
#include "ResourceIdentifiers.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <SFML/Graphics/Sprite.hpp>

class Explosion : public Entity{
    public:
        enum Type
        {
            LeftMiddle,
            LeftEnd,
            RightMiddle,
            RightEnd,
            TopMiddle,
            TopEnd,
            BottomMiddle,
            BottomEnd,
            TypeCount
        };

        Explosion(Explosion::Type type,const TextureHolder& textures);
        void drawCurrent(sf::RenderTarget &target, sf::RenderStates states) const;

        sf::FloatRect getBoundingRect() const;

        void updateCurrent(sf::Time dt, CommandQueue &commands);

        unsigned int getCategory() const;

        void updateOrigin(sf::Vector2f origin);
    private:
        Type mType;
        sf::Sprite mSprite;
};


#endif //BOMBERMAN_EXPLOSION_HPP
