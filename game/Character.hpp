#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "Entity.hpp"
#include "Command.hpp"
#include "ResourceIdentifiers.hpp"
#include "Bomb.hpp"
#include "TextNode.hpp"
#include "Animation.hpp"
#include "Tile.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include "CharacterGraph.h"


class Character : public Entity
{

public:
	enum Type
	{
		Character1,
		Character2,
		Character3,
		Character4,
		AI,
		TypeCount
	};

	Character(Character::Type type, const TextureHolder& textures, const FontHolder& fonts, int identifier, std::vector<Tile *>* tiles);
	Character(Character::Type type, const TextureHolder &textures, const FontHolder &fonts, int identifier, std::vector<Character *>* activeEnemies,
			  std::vector<Character *>* activePlayers, std::vector<Tile *>* tiles);


public:

	virtual unsigned int	getCategory() const;
	virtual sf::FloatRect	getBoundingRect() const;
	virtual void			remove();
	virtual bool 			isMarkedForRemoval() const;
	bool					isAllied() const;
	bool					isAI() const;
	float					getMaxSpeed() const;
	void					disablePickups();

	void 					increaseSpeed();
	void					increaseBombPower();
	void					increaseSpread();
	void 					increaseSpreadMax();
	void					decreaseBombDuration();
	void					collectBomb();
	int 					getSpreadLevel();

	void 					placeBomb();
	void					placeGuidedBomb();
	void					playLocalSound(CommandQueue& commands, SoundEffect::ID effect);
	int						getIdentifier();
	void					setIdentifier(int identifier);

	int						getGuidedBombAmmo() const;
	void					setGuidedBombAmmo(int ammo);
	std::vector<Bomb *>     mBombs;


private:
	virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void 			updateCurrent(sf::Time dt, CommandQueue& commands);
	void					updateMovementPattern(sf::Time dt, sf::Vector2f desiredVelocity);
	void					checkPickupDrop(CommandQueue& commands);
	void					checkBombPlacement(sf::Time dt, CommandQueue &commands);

	sf::Vector2f            getIntBlockPos() const;
	void					createBomb(SceneNode &node, const TextureHolder &textures);
	void					createPickup(SceneNode& node, const TextureHolder& textures) const;
	void 	 				updateGraph(bool);

	void					updateTexts();
	void 					updateAI(sf::Time dt, CommandQueue &commands);

	void					updateRollAnimation();
	void 					resetSpeed();

private:
	sf::Vector2f            mDesiredVelocity;
	Type					mType;
	sf::Sprite				mSprite;
	Animation				mExplosion;
	Command 				mBombCommand;
	Command 				mGuidedBombCommand;
	sf::Time				mFireCountdown;
	sf::Time 				mTimeTouched;
	bool 					mIsFiring;
	bool 					mIsPlacingBomb;
	bool 					mShowExplosion;
	bool					mExplosionBegan;
	bool					mSpawnedPickup;
	bool					mPickupsEnabled;

	int						mFireRateLevel;
	int						mSpreadLevel;
	int 					mBombAmmo;
	sf::Time				mBombDuration;
	float 					mMaxSpeed;
	CharacterGraph		    mGraph;
	std::pair<int,int>      mGoal;
	Command 				mDropPickupCommand;
	float					mTravelledDistance;
	std::size_t				mDirectionIndex;
	TextNode*				mHealthDisplay;
	TextNode*				mGuidedBombDisplay;

	int						mIdentifier;
	sf::Time				spanlife;
	sf::IntRect				mSpriteTextureRect;
	std::vector<Character *>* mActiveEnemies;
	std::vector<Character *>* mActivePlayers;
	std::vector<Tile *>* 	 mTiles;

	static const int TILES_STEP = 32.f;
};

#endif // CHARACTER_HPP