##BOMBERMAN X2013 INF585 CMAKELISTS

#Change this if you need to target a specific CMake version
cmake_minimum_required(VERSION 2.6)
project(bomberman)

set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lpthread")

#include_directories("${PROJECT_BINARY_DIR}")

#build the binaries in the specified folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

# Define sources and executable

set(GAME_FILES
        gameMain.cpp
        game/Animation.cpp
        game/Application.cpp
        game/BloomEffect.cpp
        game/Bomb.cpp
        game/Button.cpp
        game/Character.cpp
        game/Command.cpp
        game/CommandQueue.cpp
        game/Component.cpp
        game/Container.cpp
        game/DataTables.cpp
        game/EmitterNode.cpp
        game/Entity.cpp
        game/GameOverState.cpp
        game/GameServer.cpp
        game/GameState.cpp
        game/KeyBinding.cpp
        game/Label.cpp
        game/MenuState.cpp
        game/MultiplayerGameState.cpp
        game/MusicPlayer.cpp
        game/NetworkNode.cpp
        game/ParticleNode.cpp
        game/PauseState.cpp
        game/Pickup.cpp
        game/Player.cpp
        game/PostEffect.cpp
        game/SceneNode.cpp
        game/SettingsState.cpp
        game/SoundNode.cpp
        game/SoundPlayer.cpp
        game/SpriteNode.cpp
        game/State.cpp
        game/StateStack.cpp
        game/TextNode.cpp
        game/Tile.cpp
        game/TitleState.cpp
        game/Utility.cpp
        game/World.cpp
        game/RoundedRectangleShape.cpp
        game/SlideBar.cpp
        game/ModalWindow.cpp
        game/TabNavigationController.cpp
        game/NavigationBar.cpp
        game/Checkbox.cpp
        game/Preferences.cpp
        game/SettingsTabsContainers.cpp
        game/CollectionView.cpp
        game/TextBox.cpp
        game/Explosion.cpp game/CharacterGraph.cpp game/CharacterGraph.h)


add_executable(game ${GAME_FILES})

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
find_package(SFML 2 REQUIRED system window graphics network audio)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(game ${SFML_LIBRARIES})
endif ()

