#ifndef BOARD_H
#define BOARD_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Tile.h"
#include "Enums.h"

typedef struct BoardStruct {
    int size; 
    std::map<TileIdx, int> tiles;
} BoardStruct;

class Board
{
private:
    int size; 
    std::map<TileIdx, int> tiles;

public:
    Board(int size);
    ~Board();

    void addTile(Tile& tile, TileIdx tileIdx);
    int getTileId(TileIdx tileIdx);
    std::vector<int> getTileIds(TileIdx tileIdx);
    void moveTile(Tile& tile, directionType direction);
    TileIdx getTileInFront(TileIdx tileIdx, directionType direction);
    bool isOutOfBounds(TileIdx tileIdx);
    void removeTile(Tile& tile);
    TileIdx Board::getInitTileIdxFromPlayerId(int id);
    directionType getNewTileDir(worldPos& pos);
    // Network
    void getAllData(BoardStruct& m) const;
    void setAllData(BoardStruct& m);
};

#endif