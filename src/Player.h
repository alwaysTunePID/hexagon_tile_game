#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <string>
#include "Tile.h"
#include "Enums.h"

class Player
{
private:
    int points;
    int turnTime;
    std::string name;
    int id;
    TileIdx tileIdx;
    PosInTile pos;
    VelInTile vel;
    worldPos w_pos;
    worldVel w_vel;
    directionType dir;
    int spawnTileId;
    int deathCounter;
    int aimTileId;
    TileIdx aimTileIdx;
    PosInTile aimPos;
    VelInTile aimVel;
    sf::Clock clock;   // <- change this so that server doesn't need SFML
    bool currentPlayer;
    LJoyMode lJoyMode;
    bool spellOngoing;
    int selectedSpellIdx;
    directionType selectedSpellDirection;
    std::vector<SpellType> selectionSpells;
    std::vector<SpellType> discoveredSpells;

public:
    Player();
    Player(int id, TileIdx tileIdx);
    Player(int points, int turnTime, int id, bool currentPlayer);
    ~Player();

    void addPoint();
    int getPoints();
    std::string getName();
    int getId();
    TileIdx& getTileIdx();
    void setTileIdx(TileIdx tileIdxt);
    PosInTile& getPos();
    void setPos(PosInTile posT);
    worldPos getWorldPos();
    void setWorldPos(worldPos posT);
    PosInTile& getAimPos();
    void setAimPos(PosInTile posT);
    directionType getDir();
    int getSpawnTileId();
    void setSpawnTileId(int tileIdt);
    worldPos getUpdatedPos(double dt);
    PosInTile getUpdatedAimPos(double dt);
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
};

#endif