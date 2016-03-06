#include "MenuState.hpp"
#include "Button.hpp"
#include "Utility.hpp"
#include "MusicPlayer.hpp"
#include "ResourceHolder.hpp"
#include "ModalWindow.hpp"
#include "TabNavigationController.hpp"
#include "SlideBar.hpp"
#include "Checkbox.hpp"
#include "Preferences.hpp"
#include "SettingsTabsContainers.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>


MenuState::MenuState(StateStack& stack, Context context)
: State(stack, context)
, mGUIContainer()
{
	sf::Texture& texture = context.textures->get(Textures::TitleScreen);
	mBackgroundSprite.setTexture(texture);

	int buttonHorizontalPosition = 512 - 120;
	int buttonVerticalPosition = 370;
	int buttonVerticalPositionDelta = 100;

	int buttonWidth = 240;
	int buttonHeight = 60;

	auto playButton = std::make_shared<GUI::Button>(context);
	playButton->setText("New Local Game");
	playButton->setPosition(buttonHorizontalPosition, buttonVerticalPosition);
	playButton->setSize(buttonWidth, buttonHeight);
	playButton->setCallback([this] ()
	{
		requestStackPop();
		requestStackPush(States::Game);
	});
	buttonVerticalPosition += buttonVerticalPositionDelta;

	auto joinPlayButton = std::make_shared<GUI::Button>(context);
	joinPlayButton->setText("Join an Existing game");
	joinPlayButton->setPosition(buttonHorizontalPosition, buttonVerticalPosition);
	joinPlayButton->setSize(buttonWidth, buttonHeight);
	joinPlayButton->setCallback([this] () {
		requestStackPop();
		requestStackPush(States::JoinGame);
	});
	buttonVerticalPosition += buttonVerticalPositionDelta;

	auto hostPlayButton = std::make_shared<GUI::Button>(context);
	hostPlayButton->setText("Host a new Game");
	hostPlayButton->setPosition(buttonHorizontalPosition, buttonVerticalPosition);
	hostPlayButton->setSize(buttonWidth, buttonHeight);
	hostPlayButton->setCallback([this] ()
	{
		requestStackPop();
		requestStackPush(States::HostGame);
	});

	auto settingsButton = std::make_shared<GUI::Button>(context);
	settingsButton->setPosition(775, 700);
	settingsButton->setText("Settings");
	settingsButton->setCallback([this] ()
	{
		if (mSettingsModal == NULL){
			auto settings = std::make_shared<GUI::ModalWindow>(1024,768,700,500);

			settings->setPosition(0,0);
			settings->setDeactivationCallback([](){Preferences::savePrefs();});
			mSettingsModal = settings;

			auto tnc = std::make_shared<GUI::TabNavigationController>(700,500);

			auto generalContainer = tnc->addTab("General");
			auto appearanceContainer = tnc->addTab("Appearance");
			auto keymapsContainer = tnc->addTab("Keymaps");

			GUI::SettingsTabsContainers::makeGeneralTab(generalContainer);
			GUI::SettingsTabsContainers::makeAppearanceTab(appearanceContainer);
			GUI::SettingsTabsContainers::makeKeymapTab(keymapsContainer);
			tnc->activate();
			mSettingsModal->pack(tnc);
			mGUIContainer.pack(settings);

		}
		mSettingsModal->activate();
		mGUIContainer.select(5);

		//requestStackPush(States::Settings);
	});


	sf::Texture& exitTexture = context.textures->get(Textures::Exit_light);
	auto exitButton = std::make_shared<GUI::Button>(context);
	exitButton->setPosition(20, 695);
	exitButton->setText("");
	exitButton->drawRect(false);
	exitButton->drawSprite(true);
	exitButton->setCallback([this] ()
	{
		requestStackPop();
	});
	sf::Sprite* exitSprite = exitButton->getSprite();
	exitButton->setHoverTexture(&context.textures->get(Textures::Exit_light));
	exitButton->setNormalTexture(&context.textures->get(Textures::Exit_dark));

	mGUIContainer.pack(playButton);
	mGUIContainer.pack(joinPlayButton);
	mGUIContainer.pack(hostPlayButton);
	mGUIContainer.pack(settingsButton);
	mGUIContainer.pack(exitButton);

	// Play menu theme
	if(Preferences::getAsBool("playMusic"))
		context.music->play(Music::MenuTheme);
}

void MenuState::draw()
{
	sf::RenderWindow& window = *getContext().window;

	window.setView(window.getDefaultView());

	window.draw(mBackgroundSprite);
	window.draw(mGUIContainer);
}

bool MenuState::update(sf::Time dt)
{
	mGUIContainer.update(dt);
}

bool MenuState::handleEvent(const sf::Event& event)
{
	mGUIContainer.handleEvent(event);

	/*//Comes back to the game pause state
	 * //State of the Game needs to be saved
	if (isGamePaused() && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape){
		requestStackPop();
		requestStackPush(States::Game);
		requestStackPush(States::Pause);
	}
	*/

	return false;
}