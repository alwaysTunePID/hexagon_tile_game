#include <iostream>
#include <algorithm>
#include "Board.h"

Board::Board(int size)
    : size{size}, tiles{}
{}

Board::~Board()
{
}

void Board::addTile(Tile& tile, TileIdx tileIdx)
{
    tiles.insert({tileIdx, tile.getId()});
    tile.setTileIdx(tileIdx);
}

int Board::getTileId(TileIdx tileIdx)
{
    return tiles[tileIdx];
}

void Board::removeTile(Tile& tile)
{
    tiles.erase(tile.getTileIdx());
}

std::vector<int> Board::getTileIds(TileIdx tileIdx)
{
    std::vector<int> tileIds{};
    tileIds.push_back(tiles[tileIdx]);
    return tileIds;

}

void Board::moveTile(Tile& tile, directionType direction)
{
    std::cout << "CAN'T MOVE A TILE YET" << std::endl;
}

TileIdx Board::getTileInFront(TileIdx tileIdx, directionType direction)
{
    TileIdx tileInFront{ 0, 0 };
    switch (direction)
    {
    case directionType::up:
        tileInFront = {tileIdx.first - 1, tileIdx.second + 1};
        break;
    case directionType::upLeft:
        tileInFront = {tileIdx.first - 1, tileIdx.second};
        break;
    case directionType::downLeft:
        tileInFront = {tileIdx.first, tileIdx.second - 1};
        break;
    case directionType::down:
        tileInFront = {tileIdx.first + 1, tileIdx.second - 1};
        break;
    case directionType::downRight:
        tileInFront = {tileIdx.first + 1, tileIdx.second};
        break;
    case directionType::upRight:
        tileInFront = {tileIdx.first, tileIdx.second + 1};
        break;
    default:
        return OOB_TILE;
    }

    if (isOutOfBounds(tileInFront))
        return OOB_TILE;
    return tileInFront;
}

bool Board::isOutOfBounds(TileIdx tileIdx)
{
    bool result{ 
        std::abs(tileIdx.first) > size / 2 ||
        std::abs(tileIdx.second) > size / 2 ||
        std::abs(tileIdx.first + tileIdx.second) > size / 2
    };
    return result;
}

TileIdx Board::getInitTileIdxFromPlayerId(int id)
{
    TileIdx tileIdx{};
    int idx{ size / 2 };
    switch (id)
    {
    case 0:
        tileIdx = { idx, -idx };
        break;
    case 1:
        tileIdx = { -idx, idx };
        break;
    case 2:
        tileIdx = { 0, -idx };
        break;
    case 3:
        tileIdx = { 0, idx };
        break;
    case 4:
        tileIdx = { -idx, 0 };
        break;
    case 5:
        tileIdx = { idx, 0 };
        break;
    
    default:
        std::cout << "Don't support more than 6 players!" << std::endl;
        return tileIdx;
    }
    return tileIdx;
}