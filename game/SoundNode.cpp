#include "SoundNode.hpp"
#include "SoundPlayer.hpp"
#include "Preferences.hpp"


SoundNode::SoundNode(SoundPlayer& player)
: SceneNode()
, mSounds(player)
{
}

void SoundNode::playSound(SoundEffect::ID sound, sf::Vector2f position)
{
	if(Preferences::getAsBool("playSounds"))
		mSounds.play(sound, position);
}

unsigned int SoundNode::getCategory() const
{
	return Category::SoundEffect;
}
