//
// Created by Bruno Vegreville on 10/02/2016.
//

#include <ostream>
#include <iostream>
#include "Explosion.hpp"
#include "Entity.hpp"
#include "ResourceHolder.hpp"
#include "Utility.hpp"
#include "DataTables.hpp"

namespace
{
    const std::vector<ExplosionData> Table = initializeExplosionData();
}

Explosion::Explosion(Type type, const TextureHolder& textures):
        Entity(1)
        ,mType(type)
        ,mSprite(textures.get(Table[type].texture), Table[type].textureRect)
{
    centerOrigin(mSprite);
}

void Explosion::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(mSprite, states);
}

sf::FloatRect Explosion::getBoundingRect() const
{
    return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

void Explosion::updateCurrent(sf::Time dt, CommandQueue& commands)
{
}

void Explosion::updateOrigin(sf::Vector2f origin){
    mSprite.setRotation(Table[mType].angle);
    mSprite.setPosition(origin);

}

unsigned int Explosion::getCategory() const
{
    return Category::Explosion;
}