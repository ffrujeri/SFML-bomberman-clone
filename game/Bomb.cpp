#include "Bomb.hpp"
#include "EmitterNode.hpp"
#include "DataTables.hpp"
#include "Utility.hpp"
#include "ResourceHolder.hpp"
#include "Command.hpp"
#include "CommandQueue.hpp"
#include "SoundNode.hpp"
#include "NetworkNode.hpp"
#include "Explosion.hpp"
#include "Character.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <cmath>
#include <cassert>
#include <iostream>


namespace
{
	const std::vector<BombData> Table = initializeBombData();
}
const sf::Time Bomb::ExplosionDuration = sf::milliseconds(1000);

Bomb::Bomb(Type type, const TextureHolder& textures, int spreadLevel, std::vector<Tile *>* tiles)
: Entity(1)
, mType(type)
, mSprite(textures.get(Table[type].texture), Table[type].textureRect)
, mTargetDirection()
, mExplosion(textures.get(Textures::Explosion), Table[type].textureRectExplosion)
        , mExplosionAnimation(textures.get(Textures::Explosion))
, mInflate(textures.get(Textures::Bomb))
, mExplosionBegan(false)
, mShowExplosion(false)
, mSpreadLevel(spreadLevel)
{
    mCreateExplosionCommand.category = Category::SceneFieldLayer;
    mCreateExplosionCommand.action   = [this, &textures] (SceneNode& node, sf::Time)
    {
        createExplosions(node, textures);
    };

	mTimePlanted = sf::Time::Zero;
	centerOrigin(mSprite);
    mInflate.setNumFrames(10);
    mInflate.setDuration(sf::seconds(1.5));
    mInflate.setFrameSize(sf::Vector2i(32, 32));
    centerOrigin(mInflate);
    mExplosionAnimation.setNumFrames(1);
    mExplosionAnimation.setDuration(Bomb::ExplosionDuration);
    mExplosionAnimation.setFrameSize(sf::Vector2i(156,156));
    centerOrigin(mExplosion);
    centerOrigin(mExplosionAnimation);
    // Add particle system for bombs
    std::unique_ptr<EmitterNode> propellant(new EmitterNode(Particle::Propellant));
    propellant->setPosition(0.f, 0.f);
    attachChild(std::move(propellant));
    mTiles = tiles;

}

void Bomb::guideTowards(sf::Vector2f position)
{
	assert(isGuided());
	mTargetDirection = unitVector(position - getWorldPosition());
}

bool Bomb::isGuided() const
{
	return mType == GuidedBomb;
}

void Bomb::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	mTimePlanted += dt;
    mInflate.update(dt);
    checkOnTop();
    if(!isDestroyed()&&!mShowExplosion&&mTimePlanted>mTimeToLive){
		mShowExplosion = true;
	}
    if(mShowExplosion){
        if (!mExplosionBegan)
        {
            // Play sound effect
            SoundEffect::ID soundEffect = (randomInt(2) == 0) ? SoundEffect::Explosion1 : SoundEffect::Explosion2;
            playLocalSound(commands, soundEffect);

            // Emit network game action for enemy explosions
            sf::Vector2f position = getWorldPosition();

            Command command;
            command.category = Category::Network;
            command.action = derivedAction<NetworkNode>([position] (NetworkNode& node, sf::Time)
                                                        {
                                                            node.notifyGameAction(GameActions::BombExplode, position);
                                                        });

            commands.push(command);
            commands.push(mCreateExplosionCommand);
            mExplosionBegan = true;
        }
        //mExplosion.update(dt);
        if(mTimePlanted>(mTimeToLive+Bomb::ExplosionDuration)){
            mShowExplosion = false;
            destroy();
            destroyChildrenExplosions();
        }
    }
	//Entity::updateCurrent(dt, commands);
}

void Bomb::playLocalSound(CommandQueue& commands, SoundEffect::ID effect)
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

void Bomb::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if(!hasExplosionBegun()) {
		target.draw(mInflate, states);
	}
    else if (hasExplosionBegun()) {
        target.draw(mExplosion, states);
    }
}

unsigned int Bomb::getCategory() const
{
	if (mType == EnemyBomb)
		return Category::EnemyBomb;
	else
		return Category::AlliedBomb;
}

sf::FloatRect Bomb::getBoundingRect() const
{
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

float Bomb::getMaxSpeed() const
{
	return Table[mType].speed;
}

int Bomb::getDamage() const
{
	return Table[mType].damage;
}

void Bomb::setTimeToLive(sf::Time TTL) {
	mTimeToLive = TTL;
}

bool Bomb::hasExplosionBegun() const {
    return mExplosionBegan;
}

bool Bomb::isOnTop(const Character* character) const {
    for(int i = 0; i < mCharsOnTop.size(); i++)
        if(character == mCharsOnTop[i])
            return true;
    return false;
}

void Bomb::explode(){
    mShowExplosion = true;
}

void Bomb::createExplosions(SceneNode& node, const TextureHolder& textures){
    float left=1000000,right=1000000,top=1000000,bottom=1000000;
    sf::Vector2f ref = getWorldPosition();
    sf::FloatRect bounds = mInflate.getGlobalBounds();
    for(std::vector<Tile*>::iterator tile = mTiles->begin();
        tile != mTiles->end(); tile++)
    {
        sf::Vector2f ori = (*tile)->getWorldPosition();
        float distX = fabsf(ori.x-ref.x);
        float distY = fabsf(ori.y-ref.y);
        float offset = 0;
        if(distX<bounds.width/2.0){
            if((*tile)->isBlock())
                offset = bounds.height/2.0;
            if(ori.y<ref.y&&top>distY+offset){
                top = distY+offset;
            }
            else if(ori.y>ref.y && bottom>distY+offset){
                bottom = distY+offset;
            }

        }
        if(distY<bounds.height/2.0){
            offset=0;
            if((*tile)->isBlock())
                offset = bounds.width/2.0;
            if(ori.x<ref.x && left > distX+offset){
                left = distX+offset;
            }
            else if(ori.x>ref.x && right > distX+offset){
                right = distX+offset;
            }

        }
    }
    if(left==1000000|| left> mSpreadLevel*bounds.width){
        left=mSpreadLevel;
    }
    else{
        left= int(floor(left/bounds.width));
    }
    if(right==1000000 || right> mSpreadLevel*bounds.width) {
        right = mSpreadLevel;
    }
    else{
        right= int(floor(right/bounds.width));
    }
    if(top==1000000|| top> mSpreadLevel*bounds.height) {
        top = mSpreadLevel;
    }
    else{
        top= int(floor(top/bounds.height));
    }
    if(bottom==1000000 || bottom> mSpreadLevel*bounds.height) {
        bottom = mSpreadLevel;
    }
    else{
        bottom= int(floor(bottom/bounds.height));
    }
    for(int i = 1 ; i < left; i++){
        createExplosion(Explosion::LeftMiddle,node, textures,-i, 0);
    }
    if(left>=1)
        createExplosion(Explosion::LeftEnd,node, textures,-left, 0);
    for(int i = 1 ; i < right; i++){
        createExplosion(Explosion::RightMiddle,node, textures,i,0);
    }
    if(right>=1)
        createExplosion(Explosion::RightEnd,node, textures,right,0);
    for(int i = 1 ; i < top; i++){
        createExplosion(Explosion::TopMiddle, node, textures,0, -i);
    }
    if(top>=1)
        createExplosion(Explosion::TopEnd, node, textures,0, -top);
    for(int i = 1 ; i < bottom; i++){
        createExplosion(Explosion::BottomMiddle, node, textures,0, i);
    }
    if(bottom>=1)
        createExplosion(Explosion::BottomEnd, node, textures,0, bottom);
}

void Bomb::createExplosion(Explosion::Type type, SceneNode& node,const TextureHolder& textures, int horiSpriteNumber, int vertSpriteNumber) {
    std::unique_ptr<Explosion> explosion(new Explosion(type, textures));
    sf::FloatRect tmp = mInflate.getGlobalBounds();
    float xmove = tmp.width*horiSpriteNumber, ymove = tmp.height*vertSpriteNumber;
    if(xmove==0){
        if(vertSpriteNumber>0) ymove -=8;
        else if(vertSpriteNumber<0) ymove += 8;
    }
    if(ymove==0){
        if(horiSpriteNumber>0) xmove -=8;
        else if(horiSpriteNumber<0) xmove += 8;
    }
    sf::Vector2f offset(xmove,ymove);
    sf::Vector2f newPosition = mInflate.getPosition() + offset;
    explosion->updateOrigin(newPosition);
    this->attachChild(std::move(explosion));
}

int Bomb::getSpreadLevel() const {
    return mSpreadLevel;
}

void Bomb::checkOnTop() {
    for(int i = 0; i < mCharsOnTop.size(); i++) 
    {
        if(!(mCharsOnTop[i]->getBoundingRect().intersects(getBoundingRect())))
        {
            //std::cout << "Pas de collision" << std::endl;
            mCharsOnTop.erase(mCharsOnTop.begin() + i);
            i--;
        }
    }
    //std::cout << mCharsOnTop.size() << std::endl;
    //std::cout << "Hello" << std::endl;
}

