#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <string>
#include "Tile.h"
#include "WorldObject.h"
#include "Enums.h"

typedef struct PlayerStruct {
    int id;
    uint16_t characterId;
    playerStats stats;
    int turnTime;
    std::string name;
    int aimId;
    LJoyMode lJoyMode;
    int selectedSpellIdx;
    directionType selectedSpellDirection;
    std::vector<SpellType> selectionSpells;
} PlayerStruct;

class Player
{
private:
    int turnTime;
    std::string name;
    int id;
    uint16_t characterId;
    playerStats stats;
    TileIdx tileIdx;
    PosInTile pos;
    VelInTile vel;
    worldPos w_pos;
    worldPos w_vel;
    directionType dir;
    int spawnTileId;
    int aimId;
    sf::Clock clock;
    bool currentPlayer;
    LJoyMode lJoyMode;
    bool spellOngoing;
    int selectedSpellIdx;
    directionType selectedSpellDirection;
    std::vector<SpellType> selectionSpells;
    std::vector<SpellType> discoveredSpells;
    std::map<int, WorldObject>* worldObjects;

public:
    Player();
    Player(int id, uint16_t characterId, uint16_t aimId, std::string name, TileIdx tileIdx, std::map<int, WorldObject>* worldObjects);
    ~Player();

    std::string getName();
    void setName(std::string newName);
    int getId();
    playerStats getStats();
    TileIdx& getTileIdx();
    void setTileIdx(TileIdx tileIdxt);
    PosInTile& getPos();
    void setPos(PosInTile posT);
    worldPos getWorldPos();
    void setWorldPos(worldPos posT);
    directionType getDir();
    int getWorldObjectIds();
    int getSpawnTileId();
    void setSpawnTileId(int tileIdt);
    worldPos getUpdatedPos(double dt);
    void canTakeInput(bool can);
    void setVelocity(moveInput& move);
    void setAimVelocity(moveInput& move);
    bool isCurrentPlayer();
    void setCurrentPlayer(bool isCurrent);
    void updateTurnTime();
    float getTurnTime();
    bool isLJoyMode(LJoyMode mode);
    void toggleLJoyMode();
    int getAimId();
    WorldObject& getAimWO();
    SpellType getSelectedSpell();
    int getSelectedSpellIdx();
    void selectNextSpell();
    directionType getSelectedSpellDir();
    void setSelectedSpellDir(directionType direction);
    void rotateSpellDirection();
    bool isSpellOngoing();
    void setSpellOngoing(bool isOngoing);
    std::vector<SpellType>& getSelectionSpells();
    void increaseDeathCounter();
    // Network
    void getAllData(PlayerStruct& m) const;
    void setAllData(PlayerStruct& m);
};

#endif