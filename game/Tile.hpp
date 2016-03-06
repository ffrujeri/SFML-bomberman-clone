#ifndef TILE_HPP
#define TILE_HPP

#include "Entity.hpp"
#include "Command.hpp"
#include "ResourceIdentifiers.hpp"
#include "Animation.hpp"

#include <SFML/Graphics/Sprite.hpp>


class Tile : public Entity
{
	public:
		enum Type
		{
		    Floor,
            Wall,
            Block,
            TypeCount
		};


	public:
								Tile(Type type, const TextureHolder& textures);

		virtual unsigned int	getCategory() const;
		virtual sf::FloatRect	getBoundingRect() const;
		virtual void 			remove();
		virtual bool 			isMarkedForRemoval() const;
		bool 					isBlock() const;
		Type 					getType() const;

		int						getIdentifier();
		void					setIdentifier(int identifier);
		void					playLocalSound(CommandQueue& commands, SoundEffect::ID effect);


	private:
		virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
		virtual void 			updateCurrent(sf::Time dt, CommandQueue& commands);

		void					checkPickupDrop(CommandQueue& commands);
		void					createPickup(SceneNode& node, const TextureHolder& textures) const;

	private:
		Type 					mType;
		sf::Sprite				mSprite;
		Animation				mExplosion;
		bool					mShowExplosion;
		bool					mExplosionBegan;
		bool					mSpawnedPickup;
		bool					mPickupsEnabled;

		Command 				mDropPickupCommand;
		int						mIdentifier;
};

#endif // TILE_HPP
