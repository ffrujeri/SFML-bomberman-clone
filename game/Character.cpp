#include "Character.hpp"
#include "DataTables.hpp"
#include "Utility.hpp"
#include "Pickup.hpp"
#include "CommandQueue.hpp"
#include "SoundNode.hpp"
#include "NetworkNode.hpp"
#include "ResourceHolder.hpp"
#include "Explosion.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <cmath>
#include <iostream>


using namespace std::placeholders;

namespace
{
	const std::vector<CharacterData> Table = initializeCharacterData();
}

Character::Character(Type type, const TextureHolder& textures, const FontHolder& fonts, int identifier, std::vector<Tile *>* tiles)
		: Entity(Table[type].hitpoints)
		, mType(type)
		, spanlife(sf::seconds(0.0f))
		, mSpriteTextureRect(Table[type].textureRect)
		, mSprite(textures.get(Table[type].texture), Table[type].textureRect)
		, mExplosion(textures.get(Textures::Death))
		, mBombCommand()
		, mGuidedBombCommand()
		, mFireCountdown(sf::Time::Zero)
		, mIsFiring(false)
		, mIsPlacingBomb(false)
		, mShowExplosion(true)
		, mExplosionBegan(false)
		, mSpawnedPickup(false)
		, mPickupsEnabled(true)
		, mFireRateLevel(3)
		, mSpreadLevel(2)
		, mBombAmmo(2)
		, mDropPickupCommand()
		, mTravelledDistance(0.f)
		, mDirectionIndex(0)
		, mGuidedBombDisplay(nullptr)
		, mIdentifier(identifier)
		, mDesiredVelocity()
		, mBombs()
		, mGoal()
{
	mTimeTouched = sf::Time::Zero;
	mExplosion.setFrameSize(sf::Vector2i(32, 64));
	mExplosion.setNumFrames(3);
	mExplosion.setDuration(sf::seconds(1));
	sf::FloatRect bounds = mSprite.getLocalBounds();
	mSprite.setOrigin(std::floor(bounds.left + bounds.width / 2.f), std::floor(bounds.top + 3*bounds.height/4.f));
	centerOrigin(mExplosion);
	mBombCommand.category = Category::SceneFieldLayer;
	mBombCommand.action   = [this, &textures] (SceneNode& node, sf::Time)
	{
		createBomb(node, textures);
	};
	mGuidedBombCommand.category = Category::SceneFieldLayer;
	mGuidedBombCommand.action   = [this, &textures] (SceneNode& node, sf::Time)
	{
		createBomb(node, textures);
	};

	mDropPickupCommand.category = Category::SceneFieldLayer;
	mDropPickupCommand.action   = [this, &textures] (SceneNode& node, sf::Time)
	{
		createPickup(node, textures);
	};

	std::unique_ptr<TextNode> healthDisplay(new TextNode(fonts, ""));
	mHealthDisplay = healthDisplay.get();
	attachChild(std::move(healthDisplay));

	if (getCategory() == Category::PlayerCharacter)
	{
		std::unique_ptr<TextNode> guidedBombDisplay(new TextNode(fonts, ""));
		guidedBombDisplay->setPosition(0, 70);
		mGuidedBombDisplay = guidedBombDisplay.get();
		attachChild(std::move(guidedBombDisplay));
	}

	mBombDuration = sf::seconds(1.5);
	updateTexts();
	resetSpeed();
	mTiles = tiles;
}

Character::Character(Character::Type type, const TextureHolder& textures, const FontHolder& fonts, int identifier,
					 std::vector<Character *>* activeEnemies, std::vector<Character *>* activePlayers, std::vector<Tile *>* tiles): Character(type, textures, fonts, identifier, tiles) {
	mActiveEnemies = activeEnemies;
	mActivePlayers = activePlayers;
	mGraph = CharacterGraph(getWorldPosition().x+(TILES_STEP/2.f), getWorldPosition().y+(TILES_STEP/2.f), mTiles,Table[mType].AIKnowledgeRadius);
	mGoal = std::make_pair(getWorldPosition().x, getWorldPosition().y);

}
int Character::getGuidedBombAmmo() const
{
	return mBombAmmo;
}

void Character::updateGraph(bool write = false){

	mGraph = CharacterGraph(getWorldPosition().x+(TILES_STEP/2.f), getWorldPosition().y+(TILES_STEP/2.f), mTiles,Table[mType].AIKnowledgeRadius);
	if (write) {
		std::cout << "Nb of neighbors : " << mGraph.mTileMatrix.size() << std::endl;
		std::cout << " Alright " << CharacterGraph::State::Alright << std::endl;
		std::cout << " Forbidden " << CharacterGraph::State::Forbidden << std::endl;
		std::cout << " Breakable " << CharacterGraph::State::Breakable << std::endl;
		for (int i = mGraph.mTileMatrix.size()-1; i>=0; i--) {
			for (int j = 0; j < mGraph.mTileMatrix[i].size(); j++) {
				std::cout << "(" << j << "," << i << "," << mGraph.mTileMatrix[j][i] << " ) ";
			}
			std::cout << std::endl;
		}
	}

}

void Character::setGuidedBombAmmo(int ammo)
{
	mBombAmmo = ammo;
}

void Character::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if ((isDestroyed() && mExplosionBegan)){
		target.draw(mExplosion, states);
	}
	else
		target.draw(mSprite, states);
}

void Character::disablePickups()
{
	mPickupsEnabled = false;
}

void Character::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	// Update texts and roll animation
	updateTexts();
	updateRollAnimation();
	spanlife += dt;
	mTimeTouched += dt;
	// Entity has been destroyed: Possibly drop pickup, mark for removal
	if (isDestroyed())
	{
		checkPickupDrop(commands);
		mExplosion.update(dt);

		// Play explosion sound only once
		if (!mExplosionBegan)
		{
			// Play sound effect
			SoundEffect::ID soundEffect = (randomInt(2) == 0) ? SoundEffect::Explosion1 : SoundEffect::Explosion2;
			playLocalSound(commands, soundEffect);

			// Emit network game action for enemy explosions
			if (!isAllied())
			{
				sf::Vector2f position = getWorldPosition();

				Command command;
				command.category = Category::Network;
				command.action = derivedAction<NetworkNode>([position] (NetworkNode& node, sf::Time)
															{
																node.notifyGameAction(GameActions::EnemyExplode, position);
															});

				commands.push(command);
			}
			//commands.push(derivedAction<Character>(Player::CharacterMover(-1, 0, mIdentifier)));
			mExplosionBegan = true;
		}
		//if(mTimeTouched>sf::seconds(1)){
		//	mShowExplosion = false;
		//destroy();


		//	}
		return;
	}


	auto firstToRemove = std::remove_if(mBombs.begin(), mBombs.end(), std::mem_fn(&Bomb::isMarkedForRemoval));
	mBombs.erase(firstToRemove, mBombs.end());
	// Check if bullets or missiles are fired
	if(isAI()) updateAI(dt, commands);
	// Update enemy movement pattern; apply velocity
	if(!isAI()) checkBombPlacement(dt, commands);
	Entity::updateCurrent(dt, commands);
}

unsigned int Character::getCategory() const
{
	if (isAllied())
		return Category::PlayerCharacter;
	else
		return Category::EnemyCharacter;
}

int Character::getSpreadLevel() {
	return mSpreadLevel;
}
sf::FloatRect Character::getBoundingRect() const
{
	return getWorldTransform().transformRect(sf::FloatRect(mSprite.getGlobalBounds().left,mSprite.getGlobalBounds().top+34.f,mSprite.getGlobalBounds().width-4.f,mSprite.getGlobalBounds().height-34.f));
}

bool Character::isMarkedForRemoval() const
{
	return isDestroyed() && (mExplosion.isFinished() || !mShowExplosion);
}

void Character::remove()
{
	Entity::remove();
	mShowExplosion = false;
}

bool Character::isAllied() const
{
	return mType == Character1;
}

bool Character::isAI() const {
	return mType == Type::AI;
}

float Character::getMaxSpeed() const
{
	return mMaxSpeed;
}

void Character::increaseBombPower()
{
	if (mFireRateLevel < 10)
		++mFireRateLevel;
}

void Character::increaseSpread()
{
	if (mSpreadLevel < 4)
		++mSpreadLevel;
}

void Character::increaseSpreadMax()
{
	mSpreadLevel = 4;
}

void Character::increaseSpeed(){
	mMaxSpeed *= 1.5f;
}

void Character::resetSpeed(){
	mMaxSpeed = Table[mType].speed;
}

void Character::collectBomb()
{
	mBombAmmo ++;
}

void Character::placeBomb()
{
	// Only ships with fire interval != 0 are able to placeBomb
	if (Table[mType].bombInterval != sf::Time::Zero && mBombs.size()<mBombAmmo)
		mIsFiring = true;
}

void Character::placeGuidedBomb()
{
	if (mBombAmmo > 0)
	{
		mIsPlacingBomb = true;
		--mBombAmmo;
	}
}

void Character::playLocalSound(CommandQueue& commands, SoundEffect::ID effect)
{
	sf::Vector2f worldPosition = getWorldPosition();

	Command command;
	command.category = Category::SoundEffect;
	command.action = derivedAction<SoundNode>(
			[effect, worldPosition] (SoundNode& node, sf::Time)
			{
				node.playSound(effect, worldPosition);
			});

	commands.push(command);
}

int	Character::getIdentifier()
{
	return mIdentifier;
}

void Character::setIdentifier(int identifier)
{
	mIdentifier = identifier;
}

void Character::updateMovementPattern(sf::Time dt, sf::Vector2f desiredVelocity)
{
	float vx = desiredVelocity.x <= mMaxSpeed ?  desiredVelocity.x : mMaxSpeed;
	float vy = desiredVelocity.y <= mMaxSpeed ?  desiredVelocity.y : mMaxSpeed;
	setVelocity(vx, vy);
	updateRollAnimation();
}

void Character::checkPickupDrop(CommandQueue& commands)
{
	// Drop pickup, if enemy airplane, with probability 1/3, if pickup not yet dropped
	// and if not in network mode (where pickups are dropped via packets)
	if (!isAllied() && randomInt(3) == 0 && !mSpawnedPickup && mPickupsEnabled)
		commands.push(mDropPickupCommand);

	mSpawnedPickup = true;
}

void Character::checkBombPlacement(sf::Time dt, CommandQueue &commands)
{
	bool graphBomb= false;
	// Enemies try to placeBomb all the time
	if (isAI()){
		graphBomb =mGraph.placeBomb();
		if(graphBomb) placeBomb();
	}
	// Check for automatic gunfire, allow only in intervals
	if (mIsFiring && mFireCountdown <= sf::Time::Zero && mBombAmmo>mBombs.size())
	{
		// Interval expired: We can placeBomb a new bullet
		commands.push(mBombCommand);
		playLocalSound(commands, isAllied() ? SoundEffect::AlliedGunfire : SoundEffect::EnemyGunfire);

		mFireCountdown += Table[mType].bombInterval / (mFireRateLevel + 1.f);
		mIsFiring = false;
	}
	else if (mFireCountdown > sf::Time::Zero)
	{
		// Interval not expired: Decrease it further
		mFireCountdown -= dt;
		mIsFiring = false;
	}

	// Check for bomb place
	if (mIsPlacingBomb)
	{
		commands.push(mGuidedBombCommand);
		playLocalSound(commands, SoundEffect::PlaceBomb);

		mIsPlacingBomb = false;
	}
}

sf::Vector2f Character::getIntBlockPos() const
{

	sf::Vector2f centerPos(getWorldPosition().x + getBoundingRect().width / 2.f,
						   getWorldPosition().y + getBoundingRect().height / 2.f);
	int x = (int) (centerPos.x) / 32;
	int y = (int) (centerPos.y) / 32;
	return sf::Vector2f(x * 32.f, y * 32.f);
}

void Character::createBomb(SceneNode &node, const TextureHolder &textures)
{
	std::unique_ptr<Bomb> bomb(new Bomb(Bomb::AlliedBomb, textures, mSpreadLevel, mTiles));
	// Bomb position : on a specific tile
	bomb->setPosition(getIntBlockPos());
	bomb->setTimeToLive(mBombDuration);
	mBombs.push_back(bomb.get());
	node.attachChild(std::move(bomb));
}

void Character::createPickup(SceneNode& node, const TextureHolder& textures) const
{
	auto type = static_cast<Pickup::Type>(randomInt(Pickup::TypeCount));
	std::unique_ptr<Pickup> pickup(new Pickup(type, textures));
	pickup->setPosition(getIntBlockPos());
	pickup->setVelocity(0.f, 0.f);
	node.attachChild(std::move(pickup));
}

void Character::updateTexts()
{
	// Display hitpoints
	if (isDestroyed())
		mHealthDisplay->setString("Ciao");
	else
		mHealthDisplay->setString(toString(getHitpoints()) + " HP");

	mHealthDisplay->setPosition(0.f, 50.f);
	mHealthDisplay->setRotation(-getRotation());

}

void Character::updateRollAnimation()
{
	if (Table[mType].hasRollAnimation) {
		//sf::IntRect textureRect = Table[mType].textureRect;

		// Roll left: Texture rect offset once
		if (getVelocity().x < 0.f){
			mSpriteTextureRect.top = 3 * mSpriteTextureRect.height;
			if (spanlife.asSeconds() > 0.1){
				mSpriteTextureRect.left += mSpriteTextureRect.width;
				mSpriteTextureRect.left = mSpriteTextureRect.left % 96;
				spanlife = sf::seconds(0.f);
			}
		}

			// Roll right: Texture rect offset twice
		else if (getVelocity().x > 0.f){
			mSpriteTextureRect.top = mSpriteTextureRect.height;
			if (spanlife.asSeconds() > 0.1){
				mSpriteTextureRect.left += mSpriteTextureRect.width;
				mSpriteTextureRect.left = mSpriteTextureRect.left % 128;
				spanlife = sf::seconds(0.f);
			}
		}

		else if (getVelocity().y < 0.f){
			mSpriteTextureRect.top = 2 * mSpriteTextureRect.height;
			if (spanlife.asSeconds() > 0.1){
				mSpriteTextureRect.left += mSpriteTextureRect.width;
				mSpriteTextureRect.left = mSpriteTextureRect.left % 128;
				spanlife = sf::seconds(0.f);
			}
		}
		else if (getVelocity().y > 0.f){
			mSpriteTextureRect.top = 0 * mSpriteTextureRect.height;
			if (spanlife.asSeconds() > 0.1){
				mSpriteTextureRect.left += mSpriteTextureRect.width;
				mSpriteTextureRect.left = mSpriteTextureRect.left % 128;
				spanlife = sf::seconds(0.f);
			}
		}

		/*if (getVelocity().x != 0 || getVelocity().y != 0){
		int r = rand() % 1000;
		if (r < 250)
		SpriteTextureRect.left = SpriteTextureRect.width;
		else if (r < 500)
		SpriteTextureRect.left = 2 * SpriteTextureRect.width;
		}*/
		if (getVelocity().y == 0.f && getVelocity().x == 0.f)
			mSpriteTextureRect.left = 0;
		mSprite.setTextureRect(mSpriteTextureRect);

	}
}

void Character::decreaseBombDuration() {
	if(mBombDuration > sf::milliseconds(900)){
		mBombDuration -= sf::milliseconds(500);
	}
}

void Character::updateAI(sf::Time dt, CommandQueue &commands) {

	sf::Vector2f worldpos = getWorldPosition();
	int distX = abs((ceil(worldpos.x)/TILES_STEP)-mGraph.mX);
	int distY = abs((ceil(worldpos.y)/TILES_STEP)-mGraph.mY);

	if ((mGraph.mX==0 && mGraph.mY==0)||(distX>=1||distY>=1)){
		updateGraph();
		std::pair<int,int> localGoal = mGraph.getLocalCoords(mGoal.first,mGoal.second);
		CharacterGraph::Dir dir =mGraph.pathSearch(localGoal.first, localGoal.second);

		if(!mGraph.inRange(mGoal.first, mGoal.second) || dir== CharacterGraph::None){
			localGoal = mGraph.selectGoal();
			mGoal = mGraph.toAbsolute(localGoal.first, localGoal.second);
			dir = mGraph.pathSearch(localGoal.first, localGoal.second);

		}

		if(dir == CharacterGraph::Up){
			sf::Vector2f desiredVelocity(0.f, getMaxSpeed());
			mDesiredVelocity = desiredVelocity;
		}
		else if(dir == CharacterGraph::Down){
			sf::Vector2f desiredVelocity(0.f, -getMaxSpeed());
			mDesiredVelocity = desiredVelocity;
		}
		else if(dir == CharacterGraph::Left){
			sf::Vector2f desiredVelocity(-getMaxSpeed(),0.f);
			mDesiredVelocity = desiredVelocity;
		}
		else if(dir == CharacterGraph::Right){
			sf::Vector2f desiredVelocity(getMaxSpeed(), 0.f);
			mDesiredVelocity = desiredVelocity;
		}
		else{

			sf::Vector2f desiredVelocity(0.f, 0.f);
			mDesiredVelocity = desiredVelocity;
		}
		checkBombPlacement(dt, commands);

	}


	//AI WORKING HERE --- INPUT: mGraph containing information that AI can access
	//mGraph contains the list of bombs and the matrix containing the state of tiles in the neighbourhood


	//AT THIS STEP, THE DESIRED VELOCITY MUST BE SET TO UPDATE MOVEMENT
	updateMovementPattern(dt, mDesiredVelocity);
}