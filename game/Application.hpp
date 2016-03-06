#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "KeyBinding.hpp"
#include "StateStack.hpp"
#include "MusicPlayer.hpp"
#include "SoundPlayer.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>


class Application
{
	public:
		static Application* app; //global pointer toward the actual app instance to easily retrieve Holders.

	public:
								Application();
		void					run();
		TextureHolder*    		getTextureHolder();
		FontHolder* 			getFontHolder();
		MusicPlayer* 			getMusicPlayer();
		SoundPlayer* 			getSoundPlayer();
		KeyBinding* 			getKeyBinding1();
		KeyBinding* 			getKeyBinding2();

		

	private:
		void					processInput();
		void					update(sf::Time dt);
		void					render();

		void					updateStatistics(sf::Time dt);
		void					registerStates();



	private:



		static const sf::Time	TimePerFrame;

		sf::RenderWindow		mWindow;
		TextureHolder			mTextures;
	  	FontHolder				mFonts;
		MusicPlayer				mMusic;
		SoundPlayer				mSounds;
		KeyBinding				mKeyBinding1;
		KeyBinding				mKeyBinding2;
		StateStack				mStateStack;

		sf::Text				mStatisticsText;
		sf::Time				mStatisticsUpdateTime;
		std::size_t				mStatisticsNumFrames;


};



#endif // APPLICATION_HPP
