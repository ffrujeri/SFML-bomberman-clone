#include "DataTables.hpp"
#include "Character.hpp"
#include "Bomb.hpp"
#include "Tile.hpp"
#include "Pickup.hpp"
#include "Particle.hpp"
#include "Preferences.hpp"
#include "Explosion.hpp"


// For std::bind() placeholders _1, _2, ...
using namespace std::placeholders;

std::vector<CharacterData> initializeCharacterData()
{
	std::vector<CharacterData> data(Character::TypeCount);

	data[Character::Character1].hitpoints = 100;
	data[Character::Character1].speed = 120.f;
	data[Character::Character1].bombInterval = sf::seconds(1);
	data[Character::Character1].texture = Textures::Players;
	data[Character::Character1].textureRect = sf::IntRect(0 + 128, 0, 32, 64);
	data[Character::Character1].hasRollAnimation = true;

	data[Character::AI].hitpoints = 100;
	data[Character::AI].speed = 120.f;
	data[Character::AI].bombInterval = sf::seconds(1);
	data[Character::AI].texture = Textures::Players;
	data[Character::AI].textureRect = sf::IntRect(0 + 128, 0, 32, 64);
	data[Character::AI].AIKnowledgeRadius = 2;
	data[Character::AI].hasRollAnimation = true;

	data[Character::Character2].hitpoints = 100;
	data[Character::Character2].speed = 100.f;
	data[Character::Character2].bombInterval = sf::seconds(1);
	data[Character::Character2].texture = Textures::Players;
	data[Character::Character2].textureRect = sf::IntRect(128, 0, 32, 64);
	data[Character::Character2].hasRollAnimation = true;

	data[Character::Character3].hitpoints = 100;
	data[Character::Character3].speed = 100.f;
	data[Character::Character3].bombInterval = sf::seconds(1);
	data[Character::Character3].texture = Textures::Players;
	data[Character::Character3].textureRect = sf::IntRect(256, 0, 32, 64);
	data[Character::Character3].hasRollAnimation = true;

	data[Character::Character4].hitpoints = 100;
	data[Character::Character4].speed = 100.f;
	data[Character::Character4].bombInterval = sf::seconds(1);
	data[Character::Character4].texture = Textures::Players;
	data[Character::Character4].textureRect = sf::IntRect(384, 0, 32, 64);
	data[Character::Character4].hasRollAnimation = true;

	return data;
}

std::vector<BombData> initializeBombData()
{
	std::vector<BombData> data(Bomb::TypeCount);

	data[Bomb::AlliedBomb].damage = 300;
	data[Bomb::AlliedBomb].speed = 300.f;
	data[Bomb::AlliedBomb].timer = 3.f;
	data[Bomb::AlliedBomb].texture = Textures::Bomb;
	data[Bomb::AlliedBomb].textureRect = sf::IntRect(32, 0, 32, 32);
	data[Bomb::AlliedBomb].textureRectExplosion = sf::IntRect(0,8, 24, 24);

	data[Bomb::EnemyBomb].damage = 10;
	data[Bomb::EnemyBomb].speed = 300.f;
	data[Bomb::EnemyBomb].texture = Textures::Entities;
	data[Bomb::EnemyBomb].textureRect = sf::IntRect(175, 64, 12, 13);
	data[Bomb::EnemyBomb].textureRectExplosion = sf::IntRect(0,8, 24, 24);

	data[Bomb::GuidedBomb].damage = 200;
	data[Bomb::GuidedBomb].speed = 150.f;
	data[Bomb::GuidedBomb].texture = Textures::Entities;
	data[Bomb::GuidedBomb].textureRect = sf::IntRect(161, 78, 14, 14);
	data[Bomb::GuidedBomb].textureRectExplosion = sf::IntRect(0,8, 24, 24);

	return data;
}

std::vector<ExplosionData> initializeExplosionData(){
	std::vector<ExplosionData> data(Explosion::TypeCount);

	data[Explosion::LeftEnd].texture = Textures::ExplosionObject;
	data[Explosion::LeftEnd].textureRect = sf::IntRect(24, 0, 24, 33);
	data[Explosion::LeftEnd].angle = -90;

	data[Explosion::LeftMiddle].texture = Textures::ExplosionObject;
	data[Explosion::LeftMiddle].textureRect = sf::IntRect(48, 0, 24, 33);
	data[Explosion::LeftMiddle].angle = -90;

	data[Explosion::RightEnd].texture = Textures::ExplosionObject;
	data[Explosion::RightEnd].textureRect = sf::IntRect(24, 0, 24, 33);
	data[Explosion::RightEnd].angle = 90;

	data[Explosion::RightMiddle].texture = Textures::ExplosionObject;
	data[Explosion::RightMiddle].textureRect = sf::IntRect(48, 0, 24, 33);
	data[Explosion::RightMiddle].angle = 90;

	data[Explosion::TopEnd].texture = Textures::ExplosionObject;
	data[Explosion::TopEnd].textureRect = sf::IntRect(24, 0, 24, 33);
	data[Explosion::TopEnd].angle = 0;

	data[Explosion::TopMiddle].texture = Textures::ExplosionObject;
	data[Explosion::TopMiddle].textureRect = sf::IntRect(48, 0, 24, 33);
	data[Explosion::TopMiddle].angle = 180;

	data[Explosion::BottomEnd].texture = Textures::ExplosionObject;
	data[Explosion::BottomEnd].textureRect = sf::IntRect(24, 0, 24, 33);
	data[Explosion::BottomEnd].angle = 180;

	data[Explosion::BottomMiddle].texture = Textures::ExplosionObject;
	data[Explosion::BottomMiddle].textureRect = sf::IntRect(48, 0, 24, 33);
	data[Explosion::BottomMiddle].angle = 0;


	return data;
}

std::vector<PickupData> initializePickupData()
{
	std::vector<PickupData> data(Pickup::TypeCount);

	data[Pickup::FireSpread].texture = Textures::Pickups;
	data[Pickup::FireSpread].textureRect = sf::IntRect(64, 0, 33, 32);
	data[Pickup::FireSpread].action = std::bind(&Character::increaseSpread, _1);

	data[Pickup::SpeedIncrease].texture = Textures::Pickups;
	data[Pickup::SpeedIncrease].textureRect = sf::IntRect(127, 0, 33, 32);
	data[Pickup::SpeedIncrease].action = std::bind(&Character::increaseSpeed, _1);

	data[Pickup::BombIncrease].texture = Textures::Pickups;
	data[Pickup::BombIncrease].textureRect = sf::IntRect(160, 0, 33, 32);
	data[Pickup::BombIncrease].action = std::bind(&Character::collectBomb, _1);


	data[Pickup::FireMax].texture = Textures::Pickups;
	data[Pickup::FireMax].textureRect = sf::IntRect(192, 0, 33, 32);
	data[Pickup::FireMax].action = std::bind(&Character::increaseSpreadMax, _1);

	return data;
}

std::vector<TileData> initializeTileData()
{
	std::vector<TileData> data(Tile::TypeCount);

	data[Tile::Floor].solid = true;
	data[Tile::Floor].texture = Textures::World_1_Tiles;
	data[Tile::Floor].textureRect = sf::IntRect(0, 0, 32, 32);

	data[Tile::Wall].solid = true;
	data[Tile::Wall].texture = Textures::World_1_Tiles;
	data[Tile::Wall].textureRect = sf::IntRect(64, 0, 32, 32);

	data[Tile::Block].solid = true;
	data[Tile::Block].texture = Textures::World_1_Tiles;
	data[Tile::Block].textureRect = sf::IntRect(96, 0, 32, 32);

	return data;
}

std::vector<ParticleData> initializeParticleData()
{
	std::vector<ParticleData> data(Particle::ParticleCount);

	data[Particle::Propellant].color = sf::Color(255, 255, 50);
	data[Particle::Propellant].lifetime = sf::seconds(0.6f);

	data[Particle::Smoke].color = sf::Color(50, 50, 50);
	data[Particle::Smoke].lifetime = sf::seconds(4.f);

	return data;
}