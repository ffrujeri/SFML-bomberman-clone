#include "World.hpp"
#include "Bomb.hpp"
#include "Pickup.hpp"
#include "Foreach.hpp"
#include "TextNode.hpp"
#include "ParticleNode.hpp"
#include "SoundNode.hpp"
#include "NetworkNode.hpp"
#include "Utility.hpp"
#include "Explosion.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <algorithm>
#include <cmath>
#include <limits>
#include <iostream>
#include <SFML/Graphics/RectangleShape.hpp>


World::World(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds, bool networked)
		: mTarget(outputTarget)
		, mSceneTexture()
		, mWorldView(outputTarget.getDefaultView())
		, mTextures()
		, mFonts(fonts)
		, mSounds(sounds)
		, mSceneGraph()
		, mSceneLayers()
		, mWorldBounds(0.f, 0.f, mWorldView.getSize().x, mWorldView.getSize().y)
		, mSpawnPosition(mWorldView.getCenter())
		, mMapOrigin()
		, mPlayerCharacter()
		, mMap()
		, mMapCoords()
		, mEnemySpawnPoints()
		, mActiveEnemies()
		, mNetworkedWorld(networked)
		, mNetworkNode(nullptr)
		, mFinishSprite(nullptr)
{
	mSceneTexture.create(mTarget.getSize().x, mTarget.getSize().y);

	loadTextures();
	buildScene();

	// Prepare the view
	mWorldView.setCenter(mSpawnPosition);

}

void World::update(sf::Time dt)
{
	FOREACH(Character * a, mPlayerCharacter)
					a->setVelocity(0.f, 0.f);

	FOREACH(Tile * a, mMap)
					a->setVelocity(0.f, 0.f);

	FOREACH(Character * a, mActiveEnemies)
				{
					a->setVelocity(0.f,0.f) ;
				}

	// Setup commands to destroy entities, and guide missiles
	//destroyEntitiesOutsideView();
	//guideBombs();

	// Forward commands to scene graph, adapt velocity
	while (!mCommandQueue.isEmpty())
		mSceneGraph.onCommand(mCommandQueue.pop(), dt);


	adaptPlayerVelocity();


	// Remove characters that were destroyed (World::removeWrecks() only destroys the entities, not the pointers in mPlayerCharacter)
	auto playerToRemove = std::remove_if(mPlayerCharacter.begin(), mPlayerCharacter.end(), std::mem_fn(&Character::isMarkedForRemoval));
	mPlayerCharacter.erase(playerToRemove, mPlayerCharacter.end());

	auto enemyToRemove = std::remove_if(mActiveEnemies.begin(), mActiveEnemies.end(), std::mem_fn(&Character::isMarkedForRemoval));
	mActiveEnemies.erase(enemyToRemove, mActiveEnemies.end());

	auto tileToRemove = std::remove_if(mMap.begin(), mMap.end(), std::mem_fn(&Tile::isMarkedForRemoval));
	mMap.erase(tileToRemove, mMap.end());
	// Remove all destroyed entities, create new ones
	mSceneGraph.removeWrecks();
	spawnEnemies();

	// Regular update step, adapt position (correct if outside view)
	mSceneGraph.update(dt, mCommandQueue);
	//adaptPlayerPosition();
	//adaptEnemyPosition();
	updateSounds();

	// Collision detection and response (may destroy entities)
	handleCollisions(dt);
}

void World::draw()
{
	if (PostEffect::isSupported())
	{
		mSceneTexture.clear();
		mSceneTexture.setView(mWorldView);
		mSceneTexture.draw(mSceneGraph);
		mSceneTexture.display();
		mBloomEffect.apply(mSceneTexture, mTarget);
	}
	else
	{
		mTarget.setView(mWorldView);
		mTarget.draw(mSceneGraph);
	}
}

CommandQueue& World::getCommandQueue()
{
	return mCommandQueue;
}

Character * World::getCharacter(int identifier) const
{
	FOREACH(Character * a, mPlayerCharacter)
				{
					if (a->getIdentifier() == identifier)
						return a;
				}
	FOREACH(Character * a, mActiveEnemies)
				{
					if (a->getIdentifier() == identifier)
						return a;
				}
	return nullptr;
}

void World::removeCharacter(Character& character)
{

	character.destroy();

}

void World::removeCharacterById(sf::Int32 i) {
	Character* character = getCharacter(i);
	if(character){
		removeCharacter(*character);
	}
}

Character * World::addCharacter(int identifier)
{
	std::unique_ptr<Character> character(new Character(Character::Character1, mTextures, mFonts, identifier, &mMap));
	sf::Vector2f spawnPosition = mMapOrigin;
	spawnPosition.x += 32.f;
	spawnPosition.y += 32.f;
	character->setPosition(spawnPosition);	character->setIdentifier(identifier);

	mPlayerCharacter.push_back(character.get());
	mSceneLayers[Characters]->attachChild(std::move(character));
	return mPlayerCharacter.back();
}

Tile * World::addTile(Tile::Type type, float relX, float relY)
{
	std::unique_ptr<Tile> tile(new Tile(type, mTextures));
//	std::cout << mWorldView.getCenter().x << " " << mWorldView.getCenter().y << std::endl;
//	std::cout << mWorldView.getSize().x << " " << mWorldView.getSize().y << std::endl;

	tile->setPosition(mMapOrigin.x + relX, mMapOrigin.y + relY);
	mMap.push_back(tile.get());
	mSceneLayers[Tiles]->attachChild(std::move(tile));
	return mMap.back();
}

void World::createPickup(sf::Vector2f position, Pickup::Type type)
{
	std::unique_ptr<Pickup> pickup(new Pickup(type, mTextures));
	pickup->setPosition(position);
	pickup->setVelocity(0.f, 0.f);
	mSceneLayers[Pickups]->attachChild(std::move(pickup));
}

bool World::pollGameAction(GameActions::Action& out)
{
	return mNetworkNode->pollGameAction(out);
}

void World::setCurrentFieldPosition(float lineY)
{
	mWorldView.setCenter(mWorldView.getCenter().x, lineY - mWorldView.getSize().y/2);
	mSpawnPosition.y = mWorldBounds.height;
}

void World::setWorldHeight(float height)
{
	mWorldBounds.height = height;
}

bool World::hasAlivePlayer() const
{
	return mPlayerCharacter.size() > 0;
}

bool World::hasPlayerReachedEnd() const
{
	if (Character * character = getCharacter(1))
		return !mWorldBounds.contains(character->getPosition());
	else
		return false;
}

void World::loadTextures()
{
	mTextures.load(Textures::Entities, "../Media/Textures/Entities.png");
	mTextures.load(Textures::Players, "../Media/Textures/Players.png");
	mTextures.load(Textures::Death, "../Media/Textures/Death2.png");
	mTextures.load(Textures::World_1_Tiles, "../Media/Textures/World_1_tiles.png");
	mTextures.load(Textures::Bomb, "../Media/Textures/Bomb.png");
	mTextures.load(Textures::Pickups, "../Media/Textures/Pickups.png");
	mTextures.load(Textures::Field, "../Media/Textures/Field.png");
	mTextures.load(Textures::Field2, "../Media/Textures/Field2.png");
	mTextures.load(Textures::Explosion, "../Media/Textures/crosses2.png");
	mTextures.load(Textures::ExplosionObject, "../Media/Textures/crosses2.png");
	mTextures.load(Textures::Particle, "../Media/Textures/Particle.png");
	mTextures.load(Textures::FinishLine, "../Media/Textures/FinishLine.png");


}

void World::adaptPlayerPosition()
{
	// Keep player's position inside the screen bounds, at least borderDistance units from the border
	sf::FloatRect viewBounds = getViewBounds();
	const float borderDistance = 32.f;

	FOREACH(Character *hero, mPlayerCharacter)
				{
					sf::Vector2f position = hero->getPosition();
					position.x = std::max(position.x, viewBounds.left + borderDistance);
					position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
					position.y = std::max(position.y, viewBounds.top + borderDistance);
					position.y = std::min(position.y, viewBounds.top + viewBounds.height - borderDistance);
					hero->setPosition(position);
				}
}


void World::adaptEnemyPosition()
{
	// Keep enemy's position inside the screen bounds, at least borderDistance units from the border
	sf::FloatRect viewBounds = getViewBounds();
	const float borderDistance = 32.f;

	FOREACH(Character *hero, mActiveEnemies)
				{
					sf::Vector2f position = hero->getPosition();
					position.x = std::max(position.x, viewBounds.left + borderDistance);
					position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
					position.y = std::max(position.y, viewBounds.top + borderDistance);
					position.y = std::min(position.y, viewBounds.top + viewBounds.height - borderDistance);
					hero->setPosition(position);
				}
}

void World::adaptPlayerVelocity()
{
	FOREACH(Character *character, mPlayerCharacter)
				{
					sf::Vector2f velocity = character->getVelocity();

					// If moving diagonally, reduce velocity (to have always same velocity)
					if (velocity.x != 0.f && velocity.y != 0.f)
						character->setVelocity(velocity / std::sqrt(2.f));
				}
}

bool matchesCategories(SceneNode::Pair& colliders, Category::Type type1, Category::Type type2)
{
	unsigned int category1 = colliders.first->getCategory();
	unsigned int category2 = colliders.second->getCategory();

	// Make sure first pair entry has category type1 and second has type2
	if (type1 & category1 && type2 & category2)
	{
		return true;
	}
	else if (type1 & category2 && type2 & category1)
	{
		std::swap(colliders.first, colliders.second);
		return true;
	}
	else
	{
		return false;
	}
}

void World::handleCollisions(sf::Time dt)
{
	std::set<SceneNode::Pair> collisionPairs;
	//mSceneGraph.checkSceneCollision(mSceneGraph, collisionPairs);

	// Player - Pickup collisions
	mSceneLayers[Characters]->checkSceneCollision(*mSceneLayers[Pickups], collisionPairs);

	mSceneLayers[Characters]->checkSceneCollision(*mSceneLayers[Background], collisionPairs);

	mSceneLayers[Tiles]->checkSceneCollision(*mSceneLayers[Background], collisionPairs);

	mSceneLayers[Characters]->checkSceneCollision(*mSceneLayers[Tiles], collisionPairs);

	// Player - Block collisions
//	for(std::vector<Character*>::iterator it = mPlayerCharacter.begin();
//		it != mPlayerCharacter.end(); it++)
//	{
//		sf::FloatRect rect = (*it)->getBoundingRect();
//		int min_x = std::max(0, (int) ((rect.left + rect.width / 2.f) / 32.f) - 2);
//		int max_x = std::min((int) ((rect.left + rect.width / 2.f) / 32.f) + 2, NB_TILES_WIDTH - 1);
//		int min_y = std::max(0, (int) ((rect.top + rect.height / 2.f) / 32.f) - 2);
//		int max_y = std::min((int) ((rect.top + rect.height / 2.f) / 32.f) + 2, NB_TILES_HEIGHT - 1);
//		//std::cout << min_x << " ; " << max_x << std::endl;
//		//std::cout << min_y << " ; " << max_y << std::endl;
//
//		for(int i = min_x; i <= max_x; i++) {
//			for(int j = min_y; j <= max_y; j++) {
//				Tile* tmp_tile = mMapCoords[i][j];
//				if(tmp_tile != NULL)
//					tmp_tile->checkNodeCollision(*(*it), collisionPairs);
//			}
//		}
//
//		for(std::vector<Bomb*>::iterator bt = ((*it)->mBombs).begin();
//			bt != ((*it)->mBombs).end(); bt++){
//			if((*it)== nullptr) break;
//			if((*bt)!= nullptr && !(*bt)->isDestroyed()){
//				sf::FloatRect bombRect = (*bt)->getBoundingRect();
//				int spreadLevel = (*it)->getSpreadLevel();
//				int min_bomb_x = std::max(0, (int) (((bombRect.left + bombRect.width /2.f) - spreadLevel) / 32.f) - 2);
//				int max_bomb_x = std::min((int) (((bombRect.left + bombRect.width/ 2.f) + spreadLevel) / 32.f) + 2, NB_TILES_WIDTH - 1);
//				int min_bomb_y = std::max(0, (int) (((bombRect.top + bombRect.height / 2.f) -spreadLevel )/ 32.f) - 2);
//				int max_bomb_y = std::min((int) (((bombRect.top + bombRect.height / 2.f) +spreadLevel)/ 32.f) + 2, NB_TILES_HEIGHT - 1);
//				for(int i = min_bomb_x; i <= max_bomb_x; i++) {
//					for(int j = min_bomb_y; j <= max_bomb_y; j++) {
//						Tile* tmp_tile = mMapCoords[i][j];
//						if(tmp_tile != NULL){
//							(*bt)->checkNodeCollision(*tmp_tile, collisionPairs);
//						}
//
//					}
//				}
//			}
//		}
//
//	}
//
//	for(std::vector<Character*>::iterator it = mActiveEnemies.begin();
//		it != mActiveEnemies.end(); it++)
//	{
//		sf::FloatRect rect = (*it)->getBoundingRect();
//		int min_x = std::max(0, (int) ((rect.left + rect.width / 2.f) / 32.f) - 2);
//		int max_x = std::min((int) ((rect.left + rect.width / 2.f) / 32.f) + 2, NB_TILES_WIDTH - 1);
//		int min_y = std::max(0, (int) ((rect.top + rect.height / 2.f) / 32.f) - 2);
//		int max_y = std::min((int) ((rect.top + rect.height / 2.f) / 32.f) + 2, NB_TILES_HEIGHT - 1);
//		//std::cout << min_x << " ; " << max_x << std::endl;
//		//std::cout << min_y << " ; " << max_y << std::endl;
//
//		for(int i = min_x; i <= max_x; i++) {
//			for(int j = min_y; j <= max_y; j++) {
//				Tile* tmp_tile = mMapCoords[i][j];
//				if(tmp_tile != NULL)
//					tmp_tile->checkNodeCollision(*(*it), collisionPairs);
//			}
//		}
//
//		for(std::vector<Bomb*>::iterator bt = ((*it)->mBombs).begin();
//			bt != ((*it)->mBombs).end(); bt++){
//			if((*it)== nullptr) break;
//			if((*bt)!= nullptr && !(*bt)->isDestroyed()){
//				sf::FloatRect bombRect = (*bt)->getBoundingRect();
//				int spreadLevel = (*it)->getSpreadLevel();
//				int min_bomb_x = std::max(0, (int) (((bombRect.left + bombRect.width /2.f) - spreadLevel) / 32.f) - 2);
//				int max_bomb_x = std::min((int) (((bombRect.left + bombRect.width/ 2.f) + spreadLevel) / 32.f) + 2, NB_TILES_WIDTH - 1);
//				int min_bomb_y = std::max(0, (int) (((bombRect.top + bombRect.height / 2.f) -spreadLevel )/ 32.f) - 2);
//				int max_bomb_y = std::min((int) (((bombRect.top + bombRect.height / 2.f) +spreadLevel)/ 32.f) + 2, NB_TILES_HEIGHT - 1);
//				for(int i = min_bomb_x; i <= max_bomb_x; i++) {
//					for(int j = min_bomb_y; j <= max_bomb_y; j++) {
//						Tile* tmp_tile = mMapCoords[i][j];
//						if(tmp_tile != NULL){
//							(*bt)->checkNodeCollision(*tmp_tile, collisionPairs);
//						}
//
//					}
//				}
//			}
//		}
//	}


	FOREACH(SceneNode::Pair pair, collisionPairs)
				{
					if (matchesCategories(pair, Category::PlayerCharacter, Category::EnemyCharacter))
					{
						auto& player = static_cast<Character &>(*pair.first);
						auto& enemy = static_cast<Character &>(*pair.second);

						// Collision: Player damage = enemy's remaining HP
						//player.damage(enemy.getHitpoints());
						//enemy.destroy();
					}

					else if (matchesCategories(pair, Category::PlayerCharacter, Category::Pickup))
					{
						auto& player = static_cast<Character &>(*pair.first);
						auto& pickup = static_cast<Pickup&>(*pair.second);

						// Apply pickup effect to player, destroy projectile
						pickup.apply(player);
						pickup.destroy();
						player.playLocalSound(mCommandQueue, SoundEffect::CollectPickup);
					}

					else if (matchesCategories(pair, Category::Character, Category::Bomb))
					{
						auto& character = static_cast<Character &>(*pair.first);
						auto& bomb = static_cast<Bomb &>(*pair.second);
						if(bomb.hasExplosionBegun()){
							if(!character.isDestroyed()){
								removeCharacter(character);
							}
						}
					}
					else if (matchesCategories(pair, Category::Tile, Category::Bomb)){
						auto& tile = static_cast<Tile &>(*pair.first);
						auto& bomb = static_cast<Bomb &>(*pair.second);
						if(tile.isBlock()&&bomb.hasExplosionBegun()){
							tile.destroy();
							sf::Vector2f tilePosition = tile.getWorldPosition();
							int x_tile = int(tilePosition.x/32.f);
							int y_tile = int(tilePosition.y/32.f);
							mMapCoords[x_tile][y_tile] = NULL;
						}
					}
					else if (matchesCategories(pair, Category::Character, Category::Tile))
					{
						auto& character = static_cast<Character &>(*pair.first);
						auto& tile = static_cast<Tile &>(*pair.second);
						// Correction speed
						sf::Vector2f vdep(0.f, 0.f);

						sf::FloatRect c_rect = character.getBoundingRect();
						sf::FloatRect t_rect = tile.getBoundingRect();
						sf::FloatRect intersect;
						c_rect.intersects(t_rect, intersect);

						sf::Vector2f char_vel = character.getVelocity();
						/*
                        // Going right
                        if(char_vel.x >= 0)
                            if(intersect.left + intersect.width / 2.f <=
                                    t_rect.left + t_rect.width / 2.f)
                                vdep.x = -character.getVelocity().x;

                            else
                            */
						if(intersect.height*intersect.width > 64) {
							sf::Vector2f char_vel = character.getVelocity();
							sf::Vector2f corner_vect;
							corner_vect.x = t_rect.left + t_rect.width/2.f - (c_rect.left+c_rect.width/2.f);
							corner_vect.y = t_rect.top + t_rect.height/2.f - (c_rect.top+c_rect.height/2.f);
							// In this case, horizontal movement
							if (intersect.width < 2*intersect.height && corner_vect.x * char_vel.x >= 0)
								vdep.x = -character.getVelocity().x;

								// In this case, vertical movement
							else if (intersect.height < 2*intersect.width && corner_vect.y * char_vel.y >= 0 )
								vdep.y = -character.getVelocity().y;

								// In this case we are in a corner
							else {
								//Check in what corner we are

								/*
                                if(char_vel.x * corner_vect.x + corner_vect.y * char_vel.y > 0) {
                                   if (corner_vect.x * char_vel.x >= 0)
                                      vdep.x = -char_vel.x;
                                   if (corner_vect.y * char_vel.y >= 0)
                                      vdep.y = -char_vel.y;
                                }

                                /*if (char_vel.x * corner_vect.x + corner_vect.y * char_vel.y > 0) {
                                   if (abs(char_vel.x) < abs(char_vel.y))
                                      vdep.x = -char_vel.x;
                                   else if (abs(char_vel.y) < abs(char_vel.x))
                                      vdep.y = -char_vel.y;
                                   else {
                                      vdep.y = -char_vel.y;
                                      vdep.x = -char_vel.x;
                                   }
                                }*/
							}
						}



//       std::cout << character.getVelocity().x << " " << character.getVelocity().y << std::endl;
//       std::cout << character.getPosition().x << " " << character.getPosition().y << std::endl;

						character.move(vdep * dt.asSeconds());
						character.setVelocity(character.getVelocity() + vdep);
					}

					else if (matchesCategories(pair, Category::Tile, Category::Explosion)) {
						auto &tile = static_cast<Tile &>(*pair.first);
						auto &explosion = static_cast<Explosion &>(*pair.second);

						if(tile.isBlock()){
							tile.destroy();
							sf::Vector2f tilePosition = tile.getWorldPosition();
							int x_tile = int(tilePosition.x/32.f);
							int y_tile = int(tilePosition.y/32.f);
							mMapCoords[x_tile][y_tile] = NULL;
						}

					}

					else if(matchesCategories(pair, Category::Character, Category::Explosion)) {
						auto &explosion = static_cast<Explosion &>(*pair.second);
						auto &character = static_cast<Character &>(*pair.first);
						if(!character.isDestroyed()){
							removeCharacter(character);
							std::cout << "Collision" << std::endl;
						}
					}

					else if(matchesCategories(pair, Category::Explosion, Category::Bomb)){
						auto &explosion = static_cast<Explosion &>(*pair.first);
						auto &bomb = static_cast<Bomb &>(*pair.second);
						bomb.explode();
					}
				}
}

void World::updateSounds()
{
	sf::Vector2f listenerPosition;

	// 0 players (multiplayer mode, until server is connected) -> view center
	if (mPlayerCharacter.empty())
	{
		listenerPosition = mWorldView.getCenter();
	}

		// 1 or more players -> mean position between all aircrafts
	else
	{
		FOREACH(Character * aircraft, mPlayerCharacter)
						listenerPosition += aircraft->getWorldPosition();

		listenerPosition /= static_cast<float>(mPlayerCharacter.size());
	}

	// Set listener's position
	mSounds.setListenerPosition(listenerPosition);

	// Remove unused sounds
	mSounds.removeStoppedSounds();
}

void World::buildScene()
{
	// Initialize the different layers
	for (std::size_t i = 0; i < LayerCount; ++i)
	{
		Category::Type category = (i == Background) ? Category::SceneFieldLayer : Category::None;

		SceneNode::Ptr layer(new SceneNode(category));
		mSceneLayers[i] = layer.get();

		mSceneGraph.attachChild(std::move(layer));
	}
	// Prepare the tiled background
	sf::Texture& fieldTexture = mTextures.get(Textures::Field);
	fieldTexture.setRepeated(true);

	float viewHeight = mWorldView.getSize().y;
	sf::IntRect textureRect(mWorldBounds);
	textureRect.height += static_cast<int>(viewHeight);


	// Add the background sprite to the scene
	std::unique_ptr<SpriteNode> jungleSprite(new SpriteNode(fieldTexture, textureRect));
	jungleSprite->setPosition(mWorldBounds.left, mWorldBounds.top - viewHeight);
	mSceneLayers[Background]->attachChild(std::move(jungleSprite));


	// Add the tiles to the scene
	//addTile(Tile::Block, 0.f, 0.f);
	//setWall(Tile::Wall, NB_TILES_WIDTH, NB_TILES_HEIGHT);
	//addTiles(Tile::Block, NB_TILES_WIDTH, NB_TILES_HEIGHT);
	//setWall(Tile::Wall, NB_TILES_WIDTH, NB_TILES_HEIGHT);

	// we want to generate fixed map
	addWalls();
	addBlocks(100);
	//addBlocks(15);
	/*
    // Add the finish line to the scene
    sf::Texture& finishTexture = mTextures.get(Textures::FinishLine);
    std::unique_ptr<SpriteNode> finishSprite(new SpriteNode(finishTexture));
    finishSprite->setPosition(0.f, -76.f);
    mFinishSprite = finishSprite.get();
    nSceneLayers[Background]->attachChild(std::move(finishSprite));

    // Add particle node to the scene
    std::unique_ptr<ParticleNode> smokeNode(new ParticleNode(Particle::Smoke, mTextures));
    mSceneLayers[LowerAir]->attachChild(std::move(smokeNode));

    // Add propellant particle node to the scene
    std::unique_ptr<ParticleNode> propellantNode(new ParticleNode(Particle::Propellant, mTextures));
    mSceneLayers[LowerAir]->attachChild(std::move(propellantNode));

    // Add sound effect node
    std::unique_ptr<SoundNode> soundNode(new SoundNode(mSounds));
    mSceneGraph.attachChild(std::move(soundNode));
    */

	// Add network node, if necessary
	if (mNetworkedWorld)
	{
		std::unique_ptr<NetworkNode> networkNode(new NetworkNode());
		mNetworkNode = networkNode.get();
		mSceneGraph.attachChild(std::move(networkNode));
	}

	// add Pickup to Tile who are neither Wall nor Block
	while(1){
		int x1 = rand() % NB_TILES_WIDTH;
		int x2 = rand() % NB_TILES_WIDTH;
		int y1 = rand() % NB_TILES_HEIGHT;
		int y2 = rand() % NB_TILES_HEIGHT;
		if(mMapCoords[x1][y1] == NULL && mMapCoords[x2][y2] == NULL){

			createPickup(sf::Vector2f(x1*32.f, y1*32.f),Pickup::SpeedIncrease);
			createPickup(sf::Vector2f(x2*32.f, y2*32.f),Pickup::FireSpread);
			break;
		}
	}
	// Add enemy players
	addEnemies();

}

void World::addEnemies()
{
	if (mNetworkedWorld)
		return;

	// Add enemies to the spawn point container
	//addEnemy(Character::Character2, mWorldView.getCenter().x/2.f+50.f, mWorldView.getCenter().y/2.f-200);

	//addEnemy(Character::Character2, mWorldView.getCenter().x/2.f-250.f, mWorldView.getCenter().y/2.f);

	float step = 32.f;
	int ox = mSpawnPosition.x / step;
	int oy = mSpawnPosition.y / step;
	int window = 2;
	while(window < NB_TILES_HEIGHT){
		bool hasSet = false;
		while(1){
			int dx = rand() % window - window / 2;
			int dy = rand() % window - window / 2;
			if(mMapCoords[ox + dx][oy + dy] == NULL){
				addEnemy(Character::AI, (ox + dx)*step, (oy + dy)*step);
				hasSet = true;
				break;
			}
		}
		if(hasSet)
			break;
		window += 2;
	}
	//addEnemy(Character::AI, mMapOrigin.x, mMapOrigin.y-32.f);
	//addEnemy(Character::AI, mMapOrigin.x, mMapOrigin.y-32.f);

	/*
	addEnemy(Character::Character2, 0.f, 1000.f);
	addEnemy(Character::Character2, +100.f, 1150.f);
	addEnemy(Character::Character2, -100.f, 1150.f);
	addEnemy(Character::Character3, 70.f, 1500.f);
	addEnemy(Character::Character3, -70.f, 1500.f);
	addEnemy(Character::Character3, -70.f, 1710.f);
	addEnemy(Character::Character3, 70.f, 1700.f);
	addEnemy(Character::Character3, 30.f, 1850.f);
	addEnemy(Character::Character2, 300.f, 2200.f);
	addEnemy(Character::Character2, -300.f, 2200.f);
	addEnemy(Character::Character2, 0.f, 2200.f);
	addEnemy(Character::Character2, 0.f, 2500.f);
	addEnemy(Character::Character3, -300.f, 2700.f);
	addEnemy(Character::Character3, -300.f, 2700.f);
	addEnemy(Character::Character2, 0.f, 3000.f);
	addEnemy(Character::Character2, 250.f, 3250.f);
	addEnemy(Character::Character2, -250.f, 3250.f);
	addEnemy(Character::Character3, 0.f, 3500.f);
	addEnemy(Character::Character3, 0.f, 3700.f);
	addEnemy(Character::Character2, 0.f, 3800.f);
	addEnemy(Character::Character3, 0.f, 4000.f);
	addEnemy(Character::Character3, -200.f, 4200.f);
	addEnemy(Character::Character2, 200.f, 4200.f);
	addEnemy(Character::Character2, 0.f, 4400.f);
*/

	sortEnemies();
}

void World::sortEnemies()
{
	// Sort all enemies according to their y value, such that lower enemies are checked first for spawning
	std::sort(mEnemySpawnPoints.begin(), mEnemySpawnPoints.end(), [] (SpawnPoint lhs, SpawnPoint rhs)
	{
		return lhs.y < rhs.y;
	});
}

void World::addEnemy(Character::Type type, float relX, float relY)
{
	//SpawnPoint spawn(type, mSpawnPosition.x + relX, mSpawnPosition.y - relY);
	SpawnPoint spawn(type, mMapOrigin.x + relX, mMapOrigin.y + relY);
	mEnemySpawnPoints.push_back(spawn);
}

void World::spawnEnemies()
{
	// Spawn all enemies entering the view area (including distance) this frame
	while (!mEnemySpawnPoints.empty()
		   && mEnemySpawnPoints.back().y > getFieldBounds().top)
	{
		SpawnPoint spawn = mEnemySpawnPoints.back();

		int identifier = mActiveEnemies.size()+mPlayerCharacter.size();
		std::unique_ptr<Character> enemy(new Character(spawn.type, mTextures, mFonts, identifier, &mActiveEnemies, &mPlayerCharacter, &mMap));
		mActiveEnemies.push_back(enemy.get());
		enemy->setPosition(spawn.x, spawn.y);
		// enemy->setRotation(180.f);
		if (mNetworkedWorld) enemy->disablePickups();

		mSceneLayers[Characters]->attachChild(std::move(enemy));

		// Enemy is spawned, remove from the list to spawn
		mEnemySpawnPoints.pop_back();
	}
}

void World::destroyEntitiesOutsideView()
{
	Command command;
	command.category = Category::Bomb | Category::EnemyCharacter;
	command.action = derivedAction<Entity>([this] (Entity& e, sf::Time)
										   {
											   if (!getFieldBounds().intersects(e.getBoundingRect()))
												   e.remove();
										   });

	mCommandQueue.push(command);
}

void World::guideBombs()
{
	// Setup command that stores all enemies in mActiveEnemies
	Command enemyCollector;
	enemyCollector.category = Category::EnemyCharacter;
	enemyCollector.action = derivedAction<Character>([this] (Character & enemy, sf::Time)
													 {
														 if (!enemy.isDestroyed())
															 mActiveEnemies.push_back(&enemy);
													 });

	// Setup command that guides all bombs to the enemy which is currently closest to the player
	Command bombGuider;
	bombGuider.category = Category::AlliedBomb;
	bombGuider.action = derivedAction<Bomb>([this] (Bomb &bomb, sf::Time)
											{
												// Ignore unguided bullets
												if (!bomb.isGuided())
													return;

												float minDistance = std::numeric_limits<float>::max();
												Character * closestEnemy = nullptr;

												// Find closest enemy
												FOREACH(Character * enemy, mActiveEnemies)
															{
																float enemyDistance = distance(bomb, *enemy);

																if (enemyDistance < minDistance)
																{
																	closestEnemy = enemy;
																	minDistance = enemyDistance;
																}
															}

												if (closestEnemy)
													bomb.guideTowards(closestEnemy->getWorldPosition());
											});

	// Push commands, reset active enemies
	mCommandQueue.push(enemyCollector);
	mCommandQueue.push(bombGuider);
	mActiveEnemies.clear();
}

sf::FloatRect World::getViewBounds() const
{
	return sf::FloatRect(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
}

sf::FloatRect World::getFieldBounds() const
{
	// Return view bounds + some area at top, where enemies spawn
	sf::FloatRect bounds = getViewBounds();
	bounds.top -= 100.f;
	bounds.height += 100.f;

	return bounds;
}

/*void World::addTiles(Tile::Type type, const int width, const int height) {
	float step = 40.f;
void World::addWalls(Tile::Type type, const int width, const int height) {
	float step = 32.f;
	float widthStep = (mWorldView.getSize().x-step)/float(width);
	float heightStep = (mWorldView.getSize().y-step)/float(height);
	for (int i = -width/2 +1; i < (width/2); ++i) {
		for (int j = -height/2 +1; j < (height/2) ; ++j) {
			if(i!=0&&j!=0&&i%4==0&&j%4==0){
	for (int i = -width +1; i < (width); ++i) {
		for (int j = -height +1; j < (height) ; ++j) {
			if(i%4==0&&j%4==0){
				addTile(type,i*widthStep,j*heightStep);
			}
		}
	}

}*/

/*void World::addBlocks(Tile::Type type, const int width, const int height, const int proportion) {
	float step = 32.f;
	float widthStep = (mWorldView.getSize().x-step)/float(width);
	float heightStep = (mWorldView.getSize().y-step)/float(height);
	for (int i = -width +1; i < (width); ++i) {
		for (int j = -height +1; j < (height) ; ++j) {
			int rand = rand()%100;
			if(i%4==0&&j%4==0){
				addTile(type,i*widthStep,j*heightStep);
			}
		}
	}

}

void World::setWall(Tile::Type type, const int width, const int height) {
	float step = 20.f;
	float step = 32.f;
	float widthStep = (mWorldView.getSize().x-step)/float(width);
	float heightStep = (mWorldView.getSize().y-step)/float(height);
	int tmpH = height/2;
	int tmpW = width/2;
	for (int i = -tmpW; i < (tmpW) + 1; ++i) {
		addTile(type,i*widthStep,tmpH*heightStep);
		addTile(type,i*widthStep,-tmpH*heightStep);
	}

	for (int j = -tmpH; j < (tmpH) +1; ++j) {
		addTile(type,tmpW*widthStep,j*heightStep);
		addTile(type,-tmpW*widthStep,j*heightStep);
	}
}*/

void World::addTiles(Tile::Type type, int (*eligibility)[33], int probability) {
	float step = 32.f;
	for (int i = 0; i < NB_TILES_WIDTH; i++) {
		for(int j = 0; j < NB_TILES_HEIGHT; j++) {
			int random = rand() % 100;
			//if((eligibility[j][i]==1)&&(random<probability))
			if(eligibility[j][i]==1)
				mMapCoords[i][j] = addTile(type, i*step, j*step);
		}
	}
}

void World::addWalls() {
	addTiles(Tile::Wall, meligibilty_walls, 100);
}

void World::addBlocks(int probability) {
	addTiles(Tile::Block, meligibility_blocks, probability);
}