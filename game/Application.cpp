#include "Application.hpp"
#include "Utility.hpp"
#include "State.hpp"
#include "StateIdentifiers.hpp"
#include "TitleState.hpp"
#include "GameState.hpp"
#include "MultiplayerGameState.hpp"
#include "MenuState.hpp"
#include "PauseState.hpp"
#include "SettingsState.hpp"
#include "GameOverState.hpp"
#include "Preferences.hpp"


const sf::Time Application::TimePerFrame = sf::seconds(1.f/60.f);

Application* Application::app = NULL;

Application::Application()
: mWindow(sf::VideoMode(1024, 758), "Bomberman Online", sf::Style::Close,sf::ContextSettings(0,0,4))
, mStateStack(State::Context(mWindow, mTextures, mFonts, mMusic, mSounds, mKeyBinding1, mKeyBinding2))
, mStatisticsText()
, mStatisticsUpdateTime()
, mStatisticsNumFrames(0)
, mTextures()
, mFonts()
, mMusic()
, mSounds()
, mKeyBinding1(1)
, mKeyBinding2(2)
{
	app=this;
	mWindow.setKeyRepeatEnabled(false);
	mWindow.setVerticalSyncEnabled(true);

	//load preferences for the game
	Preferences::loadPrefs();

	mFonts.load(Fonts::Main, 	"../Media/Fonts/Lato-Bold.ttf");

	mTextures.load(Textures::TitleScreen,	"../Media/Textures/TitleScreen2.png");
	mTextures.load(Textures::Buttons,		"../Media/Textures/Buttons.png");
	mTextures.load(Textures::Exit_light,		"../Media/Textures/power_light.png");
	mTextures.load(Textures::Exit_dark,		"../Media/Textures/power_dark.png");
	mTextures.load(Textures::Settings_light,		"../Media/Textures/cog_light.png");
	mTextures.load(Textures::Settings_dark,		"../Media/Textures/cog_dark.png");


	mStatisticsText.setFont(mFonts.get(Fonts::Main));
	mStatisticsText.setPosition(5.f, 5.f);
	mStatisticsText.setCharacterSize(10u);

	registerStates();
	mStateStack.pushState(States::Menu);

	mSounds.setVolume(Preferences::contains("soundLevel") ? Preferences::getAsInt("soundLevel") : 25);
	mMusic.setVolume(Preferences::contains("musicLevel") ? Preferences::getAsInt("musicLevel") : 25);


	//load KeyBinding from prefs file
	for (int i =0; i<PlayerAction::Count; i++) {
		if (Preferences::contains("KM-Pl0-Act"+std::to_string(i)))
			mKeyBinding1.assignKey(static_cast<PlayerAction::Type>(i),static_cast<sf::Keyboard::Key>(Preferences::getAsInt("KM-Pl0-Act"+std::to_string(i))));
		if (Preferences::contains("KM-Pl1-Act"+std::to_string(i)))
			mKeyBinding2.assignKey(static_cast<PlayerAction::Type>(i),static_cast<sf::Keyboard::Key>(Preferences::getAsInt("KM-Pl1-Act"+std::to_string(i))));
	}
}

void Application::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;

	while (mWindow.isOpen())
	{
		sf::Time dt = clock.restart();
		timeSinceLastUpdate += dt;
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;

			processInput();
			update(TimePerFrame);

			// Check inside this loop, because stack might be empty before update() call
			if (mStateStack.isEmpty())
				mWindow.close();
		}

		updateStatistics(dt);
		render();
	}
}

void Application::processInput()
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		mStateStack.handleEvent(event);

		if (event.type == sf::Event::Closed)
			mWindow.close();
	}
}

void Application::update(sf::Time dt)
{
	mStateStack.update(dt);
}

void Application::render()
{
	mWindow.clear();

	mStateStack.draw();

	mWindow.setView(mWindow.getDefaultView());
	mWindow.draw(mStatisticsText);

	mWindow.display();
}

void Application::updateStatistics(sf::Time dt)
{
	mStatisticsUpdateTime += dt;
	mStatisticsNumFrames += 1;
	if (mStatisticsUpdateTime >= sf::seconds(1.0f))
	{
		mStatisticsText.setString("FPS: " + toString(mStatisticsNumFrames));

		mStatisticsUpdateTime -= sf::seconds(1.0f);
		mStatisticsNumFrames = 0;
	}
}

void Application::registerStates()
{
	mStateStack.registerState<TitleState>(States::Title);
	mStateStack.registerState<MenuState>(States::Menu);
	mStateStack.registerState<GameState>(States::Game);
	mStateStack.registerState<MultiplayerGameState>(States::HostGame, true);
	mStateStack.registerState<MultiplayerGameState>(States::JoinGame, false);
	mStateStack.registerState<PauseState>(States::Pause);
	mStateStack.registerState<PauseState>(States::NetworkPause, true);
	mStateStack.registerState<SettingsState>(States::Settings);
	mStateStack.registerState<GameOverState>(States::GameOver, "Game Over!");
	mStateStack.registerState<GameOverState>(States::GameSuccess, "You Won!");
}

TextureHolder *Application::getTextureHolder() {
	return &mTextures;
}

FontHolder *Application::getFontHolder() {
	return &mFonts;
}

MusicPlayer *Application::getMusicPlayer() {
	return &mMusic;
}

SoundPlayer *Application::getSoundPlayer() {
	return &mSounds;
}

KeyBinding *Application::getKeyBinding1() {
	return &mKeyBinding1;
}

KeyBinding *Application::getKeyBinding2() {
	return &mKeyBinding2;
}

