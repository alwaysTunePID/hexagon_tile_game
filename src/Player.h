#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <string>
#include "Tile.h"
#include "WorldObject.h"
#include "Enums.h"

typedef struct PlayerStruct {
    int id;
    playerStats stats;
    int turnTime;
    std::string name;
} PlayerStruct;

class Player
{
private:
    int turnTime;
    std::string name;
    int id;
    playerStats stats;
    TileIdx tileIdx;
    PosInTile pos;
    VelInTile vel;
    worldPos w_pos;
    worldPos w_vel;
    directionType dir;
    int spawnTileId;
    int aimTileId;
    TileIdx aimTileIdx;
    PosInTile aimPos;
    VelInTile aimVel;
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
    Player(int id, std::string name, TileIdx tileIdx, std::map<int, WorldObject>* worldObjects);
    Player(playerStats stats, int turnTime, int id, bool currentPlayer);
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
    PosInTile& getAimPos();
    void setAimPos(PosInTile posT);
    directionType getDir();
    int getWorldObjectIds();
    int getSpawnTileId();
    void setSpawnTileId(int tileIdt);
    worldPos getUpdatedPos(double dt);
    PosInTile getUpdatedAimPos(double dt);
    void canTakeInput(bool can);
    void setVelocity(moveInput& move);
    void setAimVelocity(moveInput& move);
    bool isCurrentPlayer();
    void setCurrentPlayer(bool isCurrent);
    void updateTurnTime();
    float getTurnTime();
    bool isLJoyMode(LJoyMode mode);
    void toggleLJoyMode();
    int getAimTileId();
    void setAimTile(Tile& tile);
    TileIdx getAimTileIdx();
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