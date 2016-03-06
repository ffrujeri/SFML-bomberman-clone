#ifndef DATATABLES_HPP
#define DATATABLES_HPP

#include "ResourceIdentifiers.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <vector>
#include <functional>


class Character;

struct Direction
{
	Direction(float angle, float distance)
			: angle(angle)
			, distance(distance)
	{
	}

	float angle;
	float distance;
};

struct CharacterData
{
	int								hitpoints;
	float							speed;
	Textures::ID					texture;
	sf::IntRect						textureRect;
	sf::Time 						bombInterval;
	int								AIKnowledgeRadius;
	bool							hasRollAnimation;
};

struct BombData
{
	int								damage;
	float							speed;
	float                           timer;
	Textures::ID					texture;
	sf::IntRect						textureRect;
	sf::IntRect						textureRectExplosion;
};

struct ExplosionData
{
	float							angle;
	Textures::ID					texture;
	sf::IntRect						textureRect;
};

struct TileData
{
	bool                            solid;
	Textures::ID                    texture;
	sf::IntRect                     textureRect;
};

struct PickupData
{
	std::function<void(Character &)>	action;
	Textures::ID					texture;
	sf::IntRect						textureRect;
};

struct ParticleData
{
	sf::Color						color;
	sf::Time						lifetime;
};


std::vector<CharacterData>	initializeCharacterData();
std::vector<BombData>   	initializeBombData();
std::vector<TileData>		initializeTileData();
std::vector<PickupData>		initializePickupData();
std::vector<ParticleData>	initializeParticleData();
std::vector<ExplosionData>	initializeExplosionData();

#endif // DATATABLES_HPP