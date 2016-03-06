#ifndef CATEGORY_HPP
#define CATEGORY_HPP


// Entity/scene node category, used to dispatch commands
namespace Category
{
	enum Type
	{
		None				= 0,
		SceneFieldLayer		= 1 << 0,
		PlayerCharacter		= 1 << 1,
		AlliedCharacter		= 1 << 2,
		EnemyCharacter		= 1 << 3,
		Pickup				= 1 << 4,
		AlliedBomb			= 1 << 5,
		EnemyBomb			= 1 << 6,
		ParticleSystem		= 1 << 7,
		SoundEffect			= 1 << 8,
		Network				= 1 << 9,
        Tile                = 1 << 10,
		Explosion			= 1 << 11,

		Character = PlayerCharacter | AlliedCharacter | EnemyCharacter,
		Bomb = AlliedBomb | EnemyBomb,
	};
}

#endif // CATEGORY_HPP
