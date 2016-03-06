//
// Created by Côme Weber on 08/02/2016.
//

#include "SettingsTabsContainers.hpp"
#include "Label.hpp"
#include "Preferences.hpp"
#include "Checkbox.hpp"
#include "Application.hpp"
#include "TextBox.hpp"
#include "CollectionView.hpp"
#include "SlideBar.hpp"
#include "Utility.hpp"

namespace GUI {

    void addButtonLabel(PlayerAction::Type p, int player, int line,const std::string& text, Container::Ptr container, std::shared_ptr<std::vector<std::shared_ptr<GUI::Label>>> keyMapsLabels){

        const float offset_v = 40.f;
        auto button = std::make_shared<GUI::Button>();
        button->setPosition(335.f*player + 60.f, 50.f*line+offset_v);
        button->setText(text);
        button->setToggle(true);

        KeyBinding* kB = player==0 ? Application::app->getKeyBinding1() : Application::app->getKeyBinding2();
        KeyBinding* kB2 = player!=0 ? Application::app->getKeyBinding1() : Application::app->getKeyBinding2();

        auto label = std::make_shared<GUI::Label>(toString(kB->getAssignedKey(p)));
        label->setPosition(335.f*player + 280.f, 50.f*line + 10.f + offset_v);

        auto weakBtn = std::weak_ptr<GUI::Button>(button);
        auto weakLab = std::weak_ptr<GUI::Label>(label);
        auto weakCont = std::weak_ptr<GUI::Container>(container);
        button->setEventHandler([=](const sf::Event& event){
            if(auto btn = weakBtn.lock()){
                if(btn->isActive()){
                    if (event.type == sf::Event::KeyReleased)
                    {
                        //check if key has not already been assigned and update GUI, KeyMap and Preferences
                        for(int i =0; i<PlayerAction::Count; i++){
                            if(kB->getAssignedKey(static_cast<PlayerAction::Type>(i))==event.key.code){
                                (*keyMapsLabels)[i]->setText("Unknown");
                                kB->assignKey(static_cast<PlayerAction::Type>(i), sf::Keyboard::Unknown);
                                Preferences::addAsInt("KM-Pl0-Act"+std::to_string(i), sf::Keyboard::Unknown);
                            }
                            if(kB2->getAssignedKey(static_cast<PlayerAction::Type>(i))==event.key.code){
                                (*keyMapsLabels)[i+PlayerAction::Count]->setText("Unknown");
                                kB2->assignKey(static_cast<PlayerAction::Type>(i), sf::Keyboard::Unknown);
                                Preferences::addAsInt("KM-Pl1-Act"+std::to_string(i),sf::Keyboard::Unknown);
                            }
                        }

                        kB->assignKey(static_cast<PlayerAction::Type>(line), event.key.code);
                        if (auto lab = weakLab.lock()) lab->setText(toString(event.key.code));
                        Preferences::addAsInt("KM-Pl"+std::to_string(player)+"-Act"+std::to_string(static_cast<int>(p)),event.key.code);

                        btn->deactivate();
                    }
                }
            }
        });
        keyMapsLabels->push_back(label);
        container->pack(button);
        container->pack(label);

    }

    void SettingsTabsContainers::makeKeymapTab(GUI::Container::Ptr container) {

        auto keymapLabels = std::make_shared<std::vector<std::shared_ptr<GUI::Label>>>();

        // Build key binding buttons and labels
        for (int x = 0; x < 2; ++x)
        {
            addButtonLabel(PlayerAction::MoveLeft,		x, 0, "Move Left",container, keymapLabels);
            addButtonLabel(PlayerAction::MoveRight,		x, 1, "Move Right",container, keymapLabels);
            addButtonLabel(PlayerAction::MoveUp,		x, 2, "Move Up",container, keymapLabels);
            addButtonLabel(PlayerAction::MoveDown,		x, 3, "Move Down",container, keymapLabels);
            addButtonLabel(PlayerAction::PlaceBomb, x, 4, "PlaceBomb",container, keymapLabels);
            addButtonLabel(PlayerAction::GuidedBomb, x, 5, "Bomb",container, keymapLabels);
        }

        auto p1Label = std::make_shared<GUI::Label>(std::string("Player 1"));
        p1Label->setPosition(130, 0);
        p1Label->setTextSize(18);
        container->pack(p1Label);

        auto p2Label = std::make_shared<GUI::Label>(std::string("Player 2"));
        p2Label->setPosition(350 + 130, 0);
        p2Label->setTextSize(18);
        container->pack(p2Label);


    }

    void SettingsTabsContainers::makeAppearanceTab(GUI::Container::Ptr container) {
        int yoffset = 50;
        auto characterlabel = std::make_shared<Label>(std::string("Select your character appearance"));
        characterlabel->setPosition(100,0+yoffset);
        container->pack(characterlabel);
        auto tiles = std::make_shared<CollectionView>();
        tiles->setPosition(0,60+yoffset);
        tiles->setSize(sf::Vector2f(700,500));
        tiles->setTileSize(sf::Vector2f(70,70));
        tiles->addTile("../Media/Textures/Players.png", sf::Vector2f(2,14), sf::Vector2f(28,48));
        tiles->addTile("../Media/Textures/Players.png", sf::Vector2f(130,14), sf::Vector2f(28,48));
        tiles->addTile("../Media/Textures/Players.png", sf::Vector2f(258,14), sf::Vector2f(28,48));
        tiles->addTile("../Media/Textures/Players.png", sf::Vector2f(386,14), sf::Vector2f(28,48));

        tiles->selectTile(Preferences::contains("PlayerSkin") ? Preferences::getAsInt("PlayerSkin") : 1);

        container->pack(tiles);

    }

    void SettingsTabsContainers::makeGeneralTab(GUI::Container::Ptr container) {

        int offset = 60;
        int offset_v = 30;
        int firstlineheight = 20 + offset_v;
        int secondLineHeight = 80 + offset_v;
        int thirdLineHeight = 170 + offset_v;
        auto musicLabel = std::make_shared<Label>(std::string("Music"));
        musicLabel->setPosition(50+offset,firstlineheight);
        auto soundLabel = std::make_shared<Label>(std::string("Sound Effects"));
        soundLabel->setPosition(50+offset,secondLineHeight);

        auto musicVolumeLabel = std::make_shared<Label>(std::string("Volume"));
        musicVolumeLabel->setPosition(250+offset,firstlineheight);
        auto soundVolumeLabel = std::make_shared<Label>(std::string("Volume"));
        soundVolumeLabel->setPosition(250+offset,secondLineHeight);

        auto musicLevelBar = std::make_shared<GUI::SlideBar>(100,200);
        musicLevelBar->showMarks(false);
        musicLevelBar->setPosition(330+offset, 2 + firstlineheight);
        musicLevelBar->setCursorPosition(Preferences::contains("musicLevel") ? Preferences::getAsInt("musicLevel") : 25);
        musicLevelBar->setCallback([](int barLevel){
            Preferences::addAsInt("musicLevel", barLevel);
            Application::app->getMusicPlayer()->setVolume(barLevel);
        });

        auto soundLevelBar = std::make_shared<GUI::SlideBar>(100,200);
        soundLevelBar->showMarks(false);
        soundLevelBar->setPosition(330+offset, 2 + secondLineHeight);
        soundLevelBar->setCursorPosition(Preferences::contains("soundLevel") ? Preferences::getAsInt("soundLevel") : 25);
        soundLevelBar->setCallback([](int barLevel){
            Preferences::addAsInt("soundLevel", barLevel);
            Application::app->getSoundPlayer()->setVolume(barLevel);
        });


        auto musicCheckbox = std::make_shared<GUI::Checkbox>(Preferences::getAsBool("playSounds"));
        musicCheckbox->setPosition(190+offset, firstlineheight);
        musicCheckbox->setCallback([](bool b){
            Preferences::addAsBool("playMusic", b);
            //if checkBox "play Music" is checked, play the music
            if (b) Application::app->getMusicPlayer()->play(Music::MenuTheme);
            else Application::app->getMusicPlayer()->stop();
        });


        auto soundCheckbox = std::make_shared<GUI::Checkbox>(Preferences::getAsBool("playMusic"));
        soundCheckbox->setPosition(190+offset, secondLineHeight);
        soundCheckbox->setCallback([](bool b){
            Preferences::addAsBool("playSounds", b);
        });


        auto ipLabel = std::make_shared<Label>(std::string("Default IP address"));
        ipLabel->setPosition(190,thirdLineHeight);


        auto ipTextBox = std::make_shared<GUI::TextBox>(Preferences::contains("ip") ? Preferences::get("ip") : "127.0.0.1");
        ipTextBox->setSize(sf::Vector2f(150,40));
        ipTextBox->setPosition(360, thirdLineHeight-10);
        auto weaktextBox = std::weak_ptr<GUI::TextBox>(ipTextBox);
        ipTextBox->setDeactivationCallback([=](){
            if (auto box = weaktextBox.lock()) { // Has to be copied into a shared_ptr before usage
               Preferences::add("ip", box->getText());
            }
        });

        container->pack(musicLabel);
        container->pack(musicCheckbox);
        container->pack(musicVolumeLabel);
        container->pack(musicLevelBar);

        container->pack(soundLabel);
        container->pack(soundCheckbox);
        container->pack(soundVolumeLabel);
        container->pack(soundLevelBar);

        container->pack(ipLabel);
        container->pack(ipTextBox);


    }

}