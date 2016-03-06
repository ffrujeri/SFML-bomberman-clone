//
// Created by fonta on 15/02/16.
//

#ifndef BOMBERMAN_CHARACTERGRAPH_H
#define BOMBERMAN_CHARACTERGRAPH_H
#include<vector>
#include "Tile.hpp"
#include "Bomb.hpp"
#include <SFML/Graphics/Sprite.hpp>

class CharacterGraph {

public:
    enum State
    {
        Dangerous,
        Forbidden,
        Alright,
        Breakable,
        Unbreakable
    };

    enum Dir
    {
        None,
        Up,
        Down,
        Left,
        Right,
    };



public:
    CharacterGraph(int x, int y, std::vector<Tile *>* tiles, int knowledgeRadius);
    CharacterGraph();
    std::pair<int, int> selectGoal();
    Dir pathSearch(int x, int y);
    bool isInNeighbourhood(sf::Vector2f pos);
    std::pair<int,int> getLocalCoords(int x, int y);
    bool placeBomb();
    bool inRange(int x, int y);
    std::pair<int,int> toAbsolute(int x, int y);
    //std::vector<GraphTile *> getTiles();


    int mX;
    int mY;
    State mState;
    std::vector<std::vector<State>> mTileMatrix;
    std::vector<Bomb> mBombs;

    int 		mKnowledgeRadius;
    static const int        HEIGHT = 24;
    static const int        WIDTH = 32;
    static const int TILES_STEP = 32.f;

};


#endif //BOMBERMAN_CHARACTERGRAPH_H