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
    int spawnTileId;
    int deathCounter;
    int aimTileId;
    TileIdx aimTileIdx;
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
    int getSpawnTileId();
    void setSpawnTileId(int tileIdt);
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