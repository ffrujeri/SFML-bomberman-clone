//
// Created by fonta on 15/02/16.
//

#include <iostream>
#include "CharacterGraph.h"
#include<queue>

/*
CharacterGraph::CharacterGraph(int x, int y,  std::vector<Tile *> *tiles) {
    a = x;
    b = y;
    GraphTile tile(x, y, tiles, &mGraphTiles, a,b);
    mGraphTiles.push_back(&tile);
    std::cout<<"END"<<std::endl;
}



std::vector<GraphTile *> CharacterGraph::getTiles() {
    return mGraphTiles;
}

*/

CharacterGraph::CharacterGraph() {

}


CharacterGraph::CharacterGraph(int x, int y, std::vector<Tile *> *tiles, int knowledgeRadius) : mKnowledgeRadius(
        knowledgeRadius) {

    mX = (x)/TILES_STEP;
    mY = (y)/TILES_STEP;
    mTileMatrix = std::vector<std::vector<State>>((unsigned long) (2 * mKnowledgeRadius + 1));
    for (int k = 0; k < mTileMatrix.size(); ++k) {
        mTileMatrix[k] = std::vector<State>((unsigned long) (2 * mKnowledgeRadius + 1));
        for (int j = 0; j < mTileMatrix.size(); j++) {
            mTileMatrix[k][j] = Alright;
        }
    }

    if(mX+mKnowledgeRadius>WIDTH){
        for(int i = WIDTH ; i < mX+mKnowledgeRadius; i++){
            for (int j = 0; j < 2*mKnowledgeRadius+1; ++j) {
                mTileMatrix[i-mX+mKnowledgeRadius][j] = Forbidden;
            }
        }
    }
    if(mX-mKnowledgeRadius<0){
        for(int i = 0 ; i > mX-mKnowledgeRadius; i--){
            for (int j = 0; j < 2*mKnowledgeRadius+1; ++j) {
                mTileMatrix[i-mX+mKnowledgeRadius][j] = Forbidden;
            }
        }
    }
    if(mY+mKnowledgeRadius>HEIGHT){
        for(int j = HEIGHT ; j < mY+mKnowledgeRadius; j++){
            for (int i = 0; i < 2*mKnowledgeRadius+1; ++i) {
                mTileMatrix[i][-j+mY+mKnowledgeRadius] = Forbidden;
            }
        }
    }
    if(mY-mKnowledgeRadius<0){
        for(int j = 0 ; j > mY-mKnowledgeRadius; j--){
            for (int i = 0; i < 2*mKnowledgeRadius+1; ++i) {
                mTileMatrix[i][-j+mY+mKnowledgeRadius] = Forbidden;
            }
        }
    }

    for(Tile *tile : *tiles){
        int tileX = (tile->getWorldPosition().x)/TILES_STEP;
        int tileY = (tile->getWorldPosition().y)/TILES_STEP;
        int distX = abs(tileX - mX);
        int distY = abs(tileY - mY);
        if(distY<=mKnowledgeRadius&&distX <=mKnowledgeRadius){
            std::pair<int,int> localCoords = getLocalCoords(tile->getWorldPosition().x, tile->getWorldPosition().y);
            if (tile->getType() == Tile::Type::Wall) {
                mTileMatrix[localCoords.first][localCoords.second] = Unbreakable;
            }
            else if (tile->getType() == Tile::Type::Block) {
                mTileMatrix[localCoords.first][localCoords.second] = Breakable;
            }
        }
    }

    for (int i = 0; i < mBombs.size(); i++) {
        if (isInNeighbourhood(mBombs[i].getPosition())) {
            std::pair<int,int> localCoords = getLocalCoords((int) mBombs[i].getPosition().x, (int) mBombs[i].getPosition().y);
            int xLoc = localCoords.first;
            int yLoc = localCoords.second;
            int range = mBombs[i].getSpreadLevel();//TODO: check if this is range
            for (int k = std::max(0, yLoc - range); k <= std::min(2 * mKnowledgeRadius, yLoc + range); k++) {
                mTileMatrix[xLoc][k] = Dangerous;
            }
            for (int k = std::max(0, xLoc - range); k <= std::min(2 * mKnowledgeRadius, xLoc + range); k++) {
                mTileMatrix[k][yLoc] = Dangerous;
            }
        }
    }
}


bool CharacterGraph::placeBomb(){
    int gk[] = {1, 0, -1, 0};
    int gr[] = {0, 1, 0, -1};
    bool place = false;
    for(int i=0;i<4;i++){
        if(mTileMatrix[mKnowledgeRadius+gk[i]][mKnowledgeRadius+gr[i]]==Breakable){
            float k = rand()%10;
            if(k<2)
                place = true;
            break;
        }
    }
    return place;

}

CharacterGraph::Dir CharacterGraph::pathSearch(int goalX, int goalY){

    if(mKnowledgeRadius == goalX && mKnowledgeRadius == goalY) {
        return None;
    }

    std::vector<std::vector<bool>> visited = std::vector<std::vector<bool>>(2 * mKnowledgeRadius + 1);
    for (int i = 0; i < mTileMatrix.size(); i++) {
        visited[i] = std::vector<bool>(mTileMatrix.size());
        for (int j = 0; j < mTileMatrix.size(); j++) {
            visited[i][j] = false;
        }
    }

    std::vector<std::vector<std::pair<int,int>>> parent = std::vector<std::vector<std::pair<int,int>>>(2 * mKnowledgeRadius + 1);
    std::pair<int,int> p(-1,-1);
    for (int i = 0; i < mTileMatrix.size(); i++) {
        parent[i] = std::vector<std::pair<int,int>>(mTileMatrix.size());
        for (int j = 0; j < mTileMatrix.size(); j++) {
            parent[i][j] = p;
        }
    }
    int gk[] = {1, 0, -1, 0};
    int gr[] = {0, 1, 0, -1};
    std::pair<int,int> origin(mKnowledgeRadius, mKnowledgeRadius);
    std::queue<int>quex, quey;
    int topx, topy, d=0;

    //memset(visited, 0, sizeof visited);
    int x = mKnowledgeRadius;
    int y = mKnowledgeRadius;
    visited[x][y] = true;

    quex.push(x);
    quey.push(y);

    while(!quex.empty()){
        topx = quex.front();
        topy = quey.front();
        quex.pop();
        quey.pop();
        if(topx == goalX && topy == goalY){
            x = topx;
            y = topy;
            while(parent[x][y]!=origin) {
                int xtemp = parent[x][y].first;
                y = parent[x][y].second;
                x = xtemp;
                //std::cout<<"direction locale "<<x<<" "<<y<<std::endl;
            }

            if(x-mKnowledgeRadius ==-1)
                return Left;
            if(y-mKnowledgeRadius ==-1)
                return Up;
            if(x-mKnowledgeRadius ==1)
                return Right;
            if(y-mKnowledgeRadius ==1)
                return Down;


        }
        for(int i = 0; i < 4; i++) {
            x = topx + gk[i];
            y = topy + gr[i];
            if(x>=0 && x<mTileMatrix.size() && y>=0 && y<mTileMatrix.size()){
                if (visited[x][y] == false && mTileMatrix[x][y] == Alright) {

                    visited[x][y] = true;

                    std::pair<int, int> p(topx, topy);

                    parent[x][y] = p;

                    quex.push(x);
                    quey.push(y);
                }
            }
        }
    }
    return None;
}


std::pair<int, int> CharacterGraph::selectGoal(){
    std::vector<std::pair<int,int>> candidates;
    for (int i = 0; i < mTileMatrix.size(); i++) {
        for (int j = 0; j < mTileMatrix.size(); j++) {
            if(mTileMatrix[i][j] == Alright){
                candidates.push_back(std::make_pair(i,j));
            }
        }
    }
    bool ok = false;
    while(ok == false && !candidates.empty()){
        int a = rand()%candidates.size();
        Dir direction = pathSearch(candidates[a].first, candidates[a].second);
        if(direction!=None){
            ok = true;
            return std::make_pair(candidates[a].first, candidates[a].second);
        }

        else
            candidates.erase(candidates.begin()+a);
    }
}

bool CharacterGraph::isInNeighbourhood(sf::Vector2f pos) {
    return abs((int) (pos.x / TILES_STEP - mX)) < mKnowledgeRadius && abs((int) (pos.y / TILES_STEP - mY)) < mKnowledgeRadius;
}

std::pair<int,int> CharacterGraph::getLocalCoords(int x, int y) {
    return std::make_pair(mKnowledgeRadius + x / TILES_STEP - mX, mKnowledgeRadius - y / TILES_STEP + mY);
}



bool CharacterGraph::inRange(int x, int y) {
    return abs(x / TILES_STEP - mX) <= mKnowledgeRadius && abs(y / TILES_STEP - mY) <= mKnowledgeRadius;
}
//
//int CharacterGraph::coordX(int x) {
//    return -mX + x / WIDTH + mKnowledgeRadius;
//}
//
//int CharacterGraph::coordY(int y) {
//    return mY - y / HEIGHT + mKnowledgeRadius;
//}

std::pair<int,int> CharacterGraph::toAbsolute(int x,int y){
    return std::make_pair((x +mX -mKnowledgeRadius)*TILES_STEP,(-y+mY+mKnowledgeRadius)*TILES_STEP);
}