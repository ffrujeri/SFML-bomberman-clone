#ifndef BOMB_HPP
#define BOMB_HPP

#include "Entity.hpp"
#include "ResourceIdentifiers.hpp"
#include "Animation.hpp"
#include "Tile.hpp"
#include "Explosion.hpp"

#include <SFML/Graphics/Sprite.hpp>
#ifdef __APPLE_CC__
#include <sys/_types/_blkcnt_t.h>
#endif

class Character;

class Bomb : public Entity
{
	public:
		enum Type
		{
			AlliedBomb,
			EnemyBomb,
			GuidedBomb,
			TypeCount
		};


	public:
								Bomb(Type type, const TextureHolder& textures, int spreadLevel, std::vector<Tile *>* tiles);

		void					guideTowards(sf::Vector2f position);
		bool					isGuided() const;
		bool 					hasExplosionBegun() const;
        bool                    isOnTop(const Character* character) const;

		virtual unsigned int	getCategory() const;
		virtual sf::FloatRect	getBoundingRect() const;
		float					getMaxSpeed() const;
		int						getDamage() const;
		int						getSpreadLevel() const;
		void 					setTimeToLive(sf::Time TTL);
		void 					playLocalSound(CommandQueue& commands, SoundEffect::ID effect);
		void					explode();
		static const sf::Time	ExplosionDuration;

	private:
		virtual void			updateCurrent(sf::Time dt, CommandQueue& commands);
		virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
        void                    checkOnTop();


	private:
		Type					mType;
		sf::Sprite				mSprite;
		sf::Vector2f			mTargetDirection;
		sf::Time 				mTimePlanted;
		sf::Time				mTimeToLive;
		bool 					mExplosionBegan;
		bool 					mShowExplosion;
		Animation 				mInflate;
		sf::Sprite				mExplosion;
		Animation				mExplosionAnimation;
		int 					mSpreadLevel;
		std::vector<Tile *>*	mTiles;
        std::vector<Character*> mCharsOnTop;
		Command 				mCreateExplosionCommand;

	void createExplosions(SceneNode &node, const TextureHolder &textures);

	void createExplosion(Explosion::Type type, SceneNode &node, const TextureHolder &textures, int horiSpriteNumber,
                         int vertSpriteNumber);


};


#endif // BOMB_HPP
