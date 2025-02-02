#include <SFML/Network/Packet.hpp>
#include "Game.h"
#include "Enums.h"
#include "WorldObject.h"
#include "Board.h"
#include "Tile.h"

/*
Packets have built-in operator >> and << overloads for standard types:
* bool
* fixed-size integer types (int8_t/16/32, uint8_t/16/32)
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
    return packet << (int8_t)m.button << (int8_t)m.action << m.move;
}

sf::Packet& operator >>(sf::Packet& packet, gameInput& m)
{
    int8_t button;
    int8_t action;
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
    return packet << m.id << (int8_t)m.type << m.pos << m.vel
                  << m.acc << (int8_t)m.dir << m.width << m.height
                  << m.origin.x << m.origin.y;
}

sf::Packet& operator >>(sf::Packet& packet, WorldObjectStruct& m)
{
    int8_t type;
    int8_t dir;

    packet = packet >> m.id >> type >> m.pos >> m.vel
                    >> m.acc >> dir >> m.width >> m.height
                    >> m.origin.x >> m.origin.y;

    m.type = (WorldObjectType)type;
    m.dir = (directionType)dir;

    return packet;
}

// WorldObjectList
sf::Packet& operator <<(sf::Packet& packet, const WorldObjectListStruct& m)
{
    packet = packet << (uint16_t)m.worldObjects.size();
    for (auto& [id, worldObjectS] : m.worldObjects)
    {
        packet = packet << worldObjectS;
    }

    return packet;
}

sf::Packet& operator >>(sf::Packet& packet, WorldObjectListStruct& m)
{
    uint16_t numOfWorldObjects;
    WorldObjectStruct worldObjectS;

    packet = packet >> numOfWorldObjects;
    for (uint16_t i{ 0 }; i < numOfWorldObjects; i++)
    {
        packet = packet >> worldObjectS;
        m.worldObjects.insert({ worldObjectS.id, worldObjectS });
    }

    return packet;
}

//TileIdx
sf::Packet& operator <<(sf::Packet& packet, const TileIdx& m)
{
    return packet << (int32_t)m.first << (int32_t)m.second;
}

sf::Packet& operator >>(sf::Packet& packet, TileIdx& m)
{
    int32_t first;
    int32_t second;

    packet = packet >> first >> second;

    m.first = (int)first;
    m.second = (int)second;

    return packet;
}

// Tile
sf::Packet& operator <<(sf::Packet& packet, const TileStruct& m)
{
    return packet << (uint16_t)m.id << m.tileIdx << (int8_t)m.dir << (int8_t)m.type
                  << (int8_t)m.effect << m.active << m.highlighted;
}

sf::Packet& operator >>(sf::Packet& packet, TileStruct& m)
{
    uint16_t id;
    int8_t   dir;
    int8_t   type;
    int8_t   effect;

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
    packet = packet << (uint16_t)m.size << (uint16_t)m.tiles.size();

    for (auto& [tileIdx, id] : m.tiles)
    {
        packet = packet << tileIdx << (uint16_t)id;
    }
    return packet;
}

sf::Packet& operator >>(sf::Packet& packet, BoardStruct& m)
{
    uint16_t size;
    uint16_t numOfTiles;
    TileIdx tileIdx;
    uint16_t id;

    packet = packet >> size >> numOfTiles;
    m.size = (int)size;

    for (uint16_t i{ 0 }; i < numOfTiles; i++)
    {
        packet = packet >> tileIdx >> id;
        m.tiles.insert({ tileIdx, (int)id });
    }

    return packet;
}

// playerStats
sf::Packet& operator <<(sf::Packet& packet, const playerStats& m)
{
    return packet << m.score << m.kills << m.deaths;
}

sf::Packet& operator >>(sf::Packet& packet, playerStats& m)
{
    return packet >> m.score >> m.kills >> m.deaths;
}

// Player
sf::Packet& operator <<(sf::Packet& packet, const PlayerStruct& m)
{
    packet = packet << (uint16_t)m.id << m.characterId << m.stats << (uint16_t)m.turnTime 
                    << m.name << (uint16_t)m.aimId << (int8_t)m.lJoyMode << (uint8_t)m.selectedSpellIdx
                    << (int8_t)m.selectedSpellDirection;

    packet = packet << (uint8_t)m.selectionSpells.size();
    for (auto spell : m.selectionSpells)
    {
        packet = packet << (int8_t)spell;
    }
    return packet;
}

sf::Packet& operator >>(sf::Packet& packet, PlayerStruct& m)
{
    uint16_t id;
    uint16_t turnTime;
    uint16_t aimId;
    int8_t   lJoyMode;
    uint8_t  selectedSpellIdx;
    int8_t   selectedSpellDirection;
    uint8_t  numOfSpells;
    int8_t   spell;

    packet = packet >> id >> m.characterId >> m.stats >> turnTime >> m.name >> aimId >> lJoyMode >> selectedSpellIdx >> selectedSpellDirection;

    m.id       = (int)id;
    m.turnTime = (int)turnTime;
    m.aimId    = (int)aimId;
    m.lJoyMode = (LJoyMode)lJoyMode;
    m.selectedSpellIdx       = (int)selectedSpellIdx;
    m.selectedSpellDirection = (directionType)selectedSpellDirection;

    packet = packet >> numOfSpells;
    for (uint8_t i{ 0 }; i < numOfSpells; i++)
    {
        packet = packet >> spell;
        m.selectionSpells.push_back((SpellType)spell);
    }

    return packet;
}

// Game
sf::Packet& operator <<(sf::Packet& packet, const GameStruct& m)
{
    packet = packet << (uint8_t)m.currentPlayer << (int8_t)m.state
                    << (uint16_t)m.boardSize << m.board;

    packet = packet << (uint16_t)m.worldObjects.size();
    for (auto& [id, worldObjectS] : m.worldObjects)
    {
        packet = packet << worldObjectS;
    }

    packet = packet << (uint8_t)m.wosToDelete.size();
    for (int id : m.wosToDelete)
    {
        packet = packet << (uint16_t)id;
    }

    packet = packet << (uint16_t)m.tiles.size();
    for (auto& [id, tileS] : m.tiles)
    {
        packet = packet << tileS;
    }

    packet = packet << (uint8_t)m.players.size();
    for (auto& [id, playerS] : m.players)
    {
        packet = packet << playerS;
    }

    return packet;
}

sf::Packet& operator >>(sf::Packet& packet, GameStruct& m)
{
    uint8_t  currentPlayer;
    int8_t   state;
    uint16_t boardSize;
    uint16_t numOfWorldObjects;
    WorldObjectStruct worldObjectS;
    uint8_t numOfWOToDelete;
    uint16_t worldObjectId;
    uint16_t numOfTiles;
    TileStruct tileS;
    uint8_t  numOfPlayers;
    PlayerStruct playerS;

    packet = packet >> currentPlayer >> state >> boardSize >> m.board;

    m.currentPlayer = (int)currentPlayer;
    m.state = (stateType)state;
    m.boardSize = (int)boardSize;

    packet = packet >> numOfWorldObjects;
    for (uint16_t i{ 0 }; i < numOfWorldObjects; i++)
    {
        packet = packet >> worldObjectS;
        m.worldObjects.insert({ worldObjectS.id, worldObjectS });
    }

    packet = packet >> numOfWOToDelete;
    for (uint8_t i{ 0 }; i < numOfWOToDelete; i++)
    {
        packet = packet >> worldObjectId;
        m.wosToDelete.push_back((int)worldObjectId);
    }

    packet = packet >> numOfTiles;
    for (uint16_t i{ 0 }; i < numOfTiles; i++)
    {
        packet = packet >> tileS;
        m.tiles.insert({ tileS.id, tileS });
    }

    packet = packet >> numOfPlayers;
    for (uint8_t i{ 0 }; i < numOfPlayers; i++)
    {
        packet = packet >> playerS;
        m.players.insert({ playerS.id, playerS });
    }

    return packet;
}