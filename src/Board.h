#ifndef BOARD_H
#define BOARD_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Tile.h"
#include "Enums.h"

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

};

#endif