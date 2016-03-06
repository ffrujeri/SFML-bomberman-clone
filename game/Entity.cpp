#include "Entity.hpp"

#include <cassert>
#include <iostream>

Entity::Entity(int hitpoints)
: mVelocity()
, mHitpoints(hitpoints)
{
}

void Entity::setVelocity(sf::Vector2f velocity)
{
	mVelocity = velocity;
}

void Entity::setVelocity(float vx, float vy)
{
	mVelocity.x = vx;
	mVelocity.y = vy;
}

sf::Vector2f Entity::getVelocity() const
{
	return mVelocity;
}

void Entity::accelerate(sf::Vector2f velocity)
{
	/*float authorized[] = {256, 512, 768};
	float TILE_RADIUS = 1024/20.0;
	float xtemp = getWorldPosition().x;
	bool exists = false;
	float dist = 0;
	for (int i = 0; i < 3; ++i) {
		if(abs(xtemp-authorized[i])<TILE_RADIUS){
			exists = true;
			dist = authorized[i];
			break;
		}
	}
	if(exists){
		velocity.x = 0;
		setPosition(dist,getWorldPosition().y);
		mVelocity += velocity;
	}
	else{
		mVelocity.x += velocity.x;
	}*/
	mVelocity += velocity;

}

void Entity::accelerate(float vx, float vy)
{
		mVelocity.x += vx;
		mVelocity.y += vy;

}

int Entity::getHitpoints() const
{
	return mHitpoints;
}

void Entity::setHitpoints(int points)
{
	assert(points > 0);
	mHitpoints = points;
}

void Entity::repair(int points)
{
	assert(points > 0);

	mHitpoints += points;
}

void Entity::damage(int points)
{
	assert(points > 0);

	mHitpoints -= points;
}

void Entity::destroy()
{
	mHitpoints = 0;
}

void Entity::remove()
{
	destroy();
}

bool Entity::isDestroyed() const
{
	return mHitpoints <= 0;
}

void Entity::updateCurrent(sf::Time dt, CommandQueue&)
{
	move(mVelocity * dt.asSeconds());
}
