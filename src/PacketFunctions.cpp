#include <SFML/Network/Packet.hpp>
#include "Game.h"
#include "Enums.h"
#include "WorldObject.h"
#include "Board.h"
#include "Tile.h"

/*
Packets have built-in operator >> and << overloads for standard types:
* bool
* fixed-size integer types (sf::Int8/16/32, sf::Uint8/16/32)
* floating point numbers (float, double)
* string types (char*, wchar_t*, std::string, std::wstring, sf::String)
*/

// moveInput
sf::Packet& operator <<(sf::Packet& packet, const moveInput& m)
{
    return packet << m.angle << m.power;
}

sf::Packet& operator >>(sf::Packet& packet, moveInput& m)
{
    return packet >> m.angle >> m.power;
}

// gameInput
sf::Packet& operator <<(sf::Packet& packet, const gameInput& m)
{
    return packet << (sf::Int8)m.button << (sf::Int8)m.action << m.move;
}

sf::Packet& operator >>(sf::Packet& packet, gameInput& m)
{
    sf::Int8 button;
    sf::Int8 action;
    packet = packet >> button >> action >> m.move;

    m.button = (xbox)button;
    m.action = (actionType)action;

    return packet;
}

// TODO change all these worldPos, worldPos.. to sf::Vector3f instead
// worldPos 
sf::Packet& operator <<(sf::Packet& packet, const worldPos& m)
{
    return packet << m.x << m.y << m.z;
}

sf::Packet& operator >>(sf::Packet& packet, worldPos& m)
{
    return packet >> m.x >> m.y >> m.z;
}

// WorldObject
sf::Packet& operator <<(sf::Packet& packet, const WorldObjectStruct& m)
{
    // TODO: add handling of this member: std::vector<Fire> effects
    return packet << m.id << (sf::Int8)m.type << m.pos << m.vel
                  << m.acc << (sf::Int8)m.dir << m.width << m.height
                  << m.origin.x << m.origin.y;
}

sf::Packet& operator >>(sf::Packet& packet, WorldObjectStruct& m)
{
    sf::Int8 type;
    sf::Int8 dir;

    packet = packet >> m.id >> type >> m.pos >> m.vel
                    >> m.acc >> dir >> m.width >> m.height
                    >> m.origin.x >> m.origin.y;

    m.type = (WorldObjectType)type;
    m.dir = (directionType)dir;

    return packet;
}

//TileIdx
sf::Packet& operator <<(sf::Packet& packet, const TileIdx& m)
{
    return packet << (sf::Int32)m.first << (sf::Int32)m.second;
}

sf::Packet& operator >>(sf::Packet& packet, TileIdx& m)
{
    sf::Int32 first;
    sf::Int32 second;

    packet = packet >> first >> second;

    m.first = (int)first;
    m.second = (int)second;

    return packet;
}

// Tile
sf::Packet& operator <<(sf::Packet& packet, const TileStruct& m)
{
    return packet << (sf::Uint16)m.id << m.tileIdx << (sf::Int8)m.dir << (sf::Int8)m.type
                  << (sf::Int8)m.effect << m.active << m.highlighted;
}

sf::Packet& operator >>(sf::Packet& packet, TileStruct& m)
{
    sf::Uint16 id;
    sf::Int8   dir;
    sf::Int8   type;
    sf::Int8   effect;

    packet = packet >> id >> m.tileIdx >> dir >> type
                    >> effect >> m.active >> m.highlighted;

    m.id     = (int)id;
    m.dir    = (directionType)dir;
    m.type   = (tileType)type;
    m.effect = (EffectType)effect;

    return packet;
}

// Board
sf::Packet& operator <<(sf::Packet& packet, const BoardStruct& m)
{
    packet = packet << (sf::Uint16)m.size << (sf::Uint16)m.tiles.size();

    for (auto& [tileIdx, id] : m.tiles)
    {
        packet = packet << tileIdx << (sf::Uint16)id;
    }
    return packet;
}

sf::Packet& operator >>(sf::Packet& packet, BoardStruct& m)
{
    sf::Uint16 size;
    sf::Uint16 numOfTiles;
    TileIdx tileIdx;
    sf::Uint16 id;

    packet = packet >> size >> numOfTiles;
    m.size = (int)size;

    for (sf::Uint16 i{ 0 }; i < numOfTiles; i++)
    {
        packet = packet >> tileIdx >> id;
        m.tiles.insert({ tileIdx, (int)id });
    }

    return packet;
}

// Player
sf::Packet& operator <<(sf::Packet& packet, const PlayerStruct& m)
{
    return packet << (sf::Uint16)m.id << (sf::Int16)m.points << (sf::Uint16)m.turnTime << m.name;
}

sf::Packet& operator >>(sf::Packet& packet, PlayerStruct& m)
{
    sf::Uint16 id;
    sf::Int16  points;
    sf::Uint16 turnTime;

    packet = packet >> id >> points >> turnTime >> m.name;

    m.id       = (int)id;
    m.points   = (int)points;
    m.turnTime = (int)turnTime;

    return packet;
}

// Game
sf::Packet& operator <<(sf::Packet& packet, const GameStruct& m)
{
    packet = packet << (sf::Uint8)m.currentPlayer << (sf::Int8)m.state
                    << (sf::Uint16)m.boardSize << m.board;

    packet = packet << (sf::Uint16)m.worldObjects.size();
    for (auto& [id, worldObjectS] : m.worldObjects)
    {
        packet = packet << worldObjectS;
    }

    packet = packet << (sf::Uint16)m.tiles.size();
    for (auto& [id, tileS] : m.tiles)
    {
        packet = packet << tileS;
    }

    packet = packet << (sf::Uint8)m.players.size();
    for (auto& [id, playerS] : m.players)
    {
        packet = packet << playerS;
    }

    return packet;
}

sf::Packet& operator >>(sf::Packet& packet, GameStruct& m)
{
    sf::Uint8  currentPlayer;
    sf::Int8   state;
    sf::Uint16 boardSize;
    sf::Uint16 numOfWorldObjects;
    WorldObjectStruct worldObjectS;
    sf::Uint16 numOfTiles;
    TileStruct tileS;
    sf::Uint8  numOfPlayers;
    PlayerStruct playerS;

    packet = packet >> currentPlayer >> state >> boardSize >> m.board;

    m.currentPlayer = (int)currentPlayer;
    m.state = (stateType)state;
    m.boardSize = (int)boardSize;

    packet = packet >> numOfWorldObjects;
    for (sf::Uint16 i{ 0 }; i < numOfWorldObjects; i++)
    {
        packet = packet >> worldObjectS;
        m.worldObjects.insert({ worldObjectS.id, worldObjectS });
    }

    packet = packet >> numOfTiles;
    for (sf::Uint16 i{ 0 }; i < numOfTiles; i++)
    {
        packet = packet >> tileS;
        m.tiles.insert({ tileS.id, tileS });
    }

    packet = packet >> numOfPlayers;
    for (sf::Uint8 i{ 0 }; i < numOfPlayers; i++)
    {
        packet = packet >> playerS;
        m.players.insert({ playerS.id, playerS });
    }

    return packet;
}