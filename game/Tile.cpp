#include "Tile.hpp"
#include "DataTables.hpp"
#include "Pickup.hpp"
#include "CommandQueue.hpp"
#include "Utility.hpp"
#include "SoundNode.hpp"
#include "NetworkNode.hpp"
#include "ResourceHolder.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <iostream>


namespace
{
	const std::vector<TileData> Table = initializeTileData();
}

Tile::Tile(Type type, const TextureHolder& textures)
: Entity(1)
, mType(type)
, mSprite(textures.get(Table[type].texture), Table[type].textureRect)
, mExplosion(textures.get(Textures::Explosion))
, mDropPickupCommand()
, mShowExplosion(false)
, mSpawnedPickup(false)
, mPickupsEnabled(true)
{
	mExplosion.setFrameSize(sf::Vector2i(256, 256));
	mExplosion.setNumFrames(16);
	mExplosion.setDuration(sf::seconds(1));

	centerOrigin(mSprite);
	centerOrigin(mExplosion);

	mDropPickupCommand.category = Category::SceneFieldLayer;
	mDropPickupCommand.action   = [this, &textures] (SceneNode& node, sf::Time)
	{
		createPickup(node, textures);
	};


}

void Tile::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (isDestroyed()){
		//target.draw(mExplosion, states);
	}
	else
		target.draw(mSprite, states);
}

void Tile::updateCurrent(sf::Time dt, CommandQueue& commands)
{
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

			// Emit network game action for explosions
			if (isBlock())
			{
				sf::Vector2f position = getWorldPosition();

				Command command;
				command.category = Category::Network;
				command.action = derivedAction<NetworkNode>([position] (NetworkNode& node, sf::Time)
															{
																node.notifyGameAction(GameActions::EnemyExplode, position);
															});

				commands.push(command);
				mExplosionBegan = true;
			}
		}
		return;
	}

	Entity::updateCurrent(dt, commands);
}

unsigned int Tile::getCategory() const
{
	return Category::Tile;
}

sf::FloatRect Tile::getBoundingRect() const
{
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

bool Tile::isMarkedForRemoval() const
{
	return isDestroyed() && (mExplosion.isFinished() || mShowExplosion);
}

void Tile::remove()
{
	Entity::remove();
}

bool Tile::isBlock() const
{
	return mType == Block;
}

void Tile::playLocalSound(CommandQueue& commands, SoundEffect::ID effect)
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

int	Tile::getIdentifier()
{
	return mIdentifier;
}

void Tile::setIdentifier(int identifier)
{
	mIdentifier = identifier;
}

void Tile::checkPickupDrop(CommandQueue& commands)
{
	// Drop pickup, if enemy airplane, with probability 1/4, if pickup not yet dropped
	// and if not in network mode (where pickups are dropped via packets)
	if (isBlock() && randomInt(5) == 0 && !mSpawnedPickup && mPickupsEnabled){
		commands.push(mDropPickupCommand);
	}
	mSpawnedPickup = true;
}

void Tile::createPickup(SceneNode& node, const TextureHolder& textures) const
{
	auto type = static_cast<Pickup::Type>(randomInt(Pickup::TypeCount));

	std::unique_ptr<Pickup> pickup(new Pickup(type, textures));
	pickup->setPosition(getWorldPosition());
	pickup->setVelocity(0.f, 0.f);
	node.attachChild(std::move(pickup));
}


Tile::Type Tile::getType() const {
	return mType;
}
