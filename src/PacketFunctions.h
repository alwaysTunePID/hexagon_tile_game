#ifndef PACKETFUNCTIONS_H
#define PACKETFUNCTIONS_H

enum class PackageType
{
    delta,
    all,
    last
};

// moveInput
sf::Packet& operator <<(sf::Packet& packet, const moveInput& m);
sf::Packet& operator >>(sf::Packet& packet, moveInput& m);
// gameInput
sf::Packet& operator <<(sf::Packet& packet, const gameInput& m);
sf::Packet& operator >>(sf::Packet& packet, gameInput& m);
// worldPos 
sf::Packet& operator <<(sf::Packet& packet, const worldPos& m);
sf::Packet& operator >>(sf::Packet& packet, worldPos& m);
// WorldObject
sf::Packet& operator <<(sf::Packet& packet, const WorldObjectStruct& m);
sf::Packet& operator >>(sf::Packet& packet, WorldObjectStruct& m);
// WorldObjectList
sf::Packet& operator <<(sf::Packet& packet, const WorldObjectListStruct& m);
sf::Packet& operator >>(sf::Packet& packet, WorldObjectListStruct& m);
//TileIdx
sf::Packet& operator <<(sf::Packet& packet, const TileIdx& m);
sf::Packet& operator >>(sf::Packet& packet, TileIdx& m);
// Tile
sf::Packet& operator <<(sf::Packet& packet, const TileStruct& m);
sf::Packet& operator >>(sf::Packet& packet, TileStruct& m);
// Board
sf::Packet& operator <<(sf::Packet& packet, const BoardStruct& m);
sf::Packet& operator >>(sf::Packet& packet, BoardStruct& m);
// playerStats
sf::Packet& operator <<(sf::Packet& packet, const playerStats& m);
sf::Packet& operator >>(sf::Packet& packet, playerStats& m);
// Player
sf::Packet& operator <<(sf::Packet& packet, const PlayerStruct& m);
sf::Packet& operator >>(sf::Packet& packet, PlayerStruct& m);
// Game
sf::Packet& operator <<(sf::Packet& packet, const GameStruct& m);
sf::Packet& operator >>(sf::Packet& packet, GameStruct& m);

#endif