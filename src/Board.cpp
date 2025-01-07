#include <iostream>
#include <algorithm>
#include <cmath>
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

directionType Board::getNewTileDir(worldPos& pos)
{
    /*
    // Test square first
    if (std::abs(pos.first) < 12.f && 
        std::abs(pos.second) < static_cast<float>(SQ_HEIGHT / 2.0))
    {
        //std::cout << "x: " << pos.first << " " << pos.second << std::endl;
        return directionType::none;
    }

    //    12    arctan (18/12) = 0,983
    //      _ _ 
    //    /\  |
    //   /  \ | 18
    //  /_  _\|

    //  f(12) = 18 , 18 = k * 12 + m  =>  17 = - k * 12  => k = - 17 / 12 , m = 35 
    //  f(24) = 1  , 1  = k * 24 + m

    //  width diff = SQ_WIDTH * 0.75
    //  height diff = SQ_HEIGHT * 0.5
    

    float ang1{ 0.983 };
    float k{ - 17 / 12 };
    float m{ 35 };
    float widthDiff{ static_cast<float>(SQ_WIDTH * 0.75) };
    float heightDiff{ static_cast<float>(SQ_HEIGHT * 0.5) };

    // Handle special case x = 0
    if (pos.first == 0.f){
        return (pos.second > 0) ? directionType::down : directionType::up;
    }

    float angle{ atan2(pos.second, pos.first) };
    if (angle > 0.f && angle <= ang1)
    {
        if (pos.second > k * pos.first + m)
        {
            pos.first += -widthDiff;
            pos.second += -heightDiff;
            return directionType::downRight;
        }
    }
    else if (angle > ang1 && angle <= PI - ang1)
    {
        pos.second += -static_cast<float>(SQ_HEIGHT);
        return directionType::down;
    }
    else if (angle > PI - ang1 && angle <= PI)
    {
        if (pos.second > k * -pos.first + m)
        {
            pos.first += widthDiff;
            pos.second += -heightDiff;
            return directionType::downLeft;
        }
    }
    else if (angle < -PI + ang1 && angle >= -PI)
    {
        if (-pos.second > k * -pos.first + m)
        {
            pos.first += widthDiff;
            pos.second += heightDiff;
            return directionType::upLeft;
        }
    }
    else if (angle < -ang1 && angle >= -PI + ang1)
    {
        pos.second += static_cast<float>(SQ_HEIGHT);
        return directionType::up;
    }
    else if (angle < 0 && angle >= -ang1)
    {
        if (-pos.second > k * pos.first + m)
        {
            pos.first += -widthDiff;
            pos.second += heightDiff;
            return directionType::upRight;
        }
    }*/
    return directionType::none;
}

// Network
void Board::getAllData(BoardStruct& m) const
{
    m.size  = size;
    m.tiles = tiles;
}

void Board::setAllData(BoardStruct& m)
{
    size  = m.size;
    tiles = m.tiles;
}