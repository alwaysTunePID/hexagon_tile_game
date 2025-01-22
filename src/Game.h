#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <chrono>
#include <vector>
#include <map>
#include <random>
#include "Tile.h"
#include "WorldObject.h"
#include "Board.h"
#include "Player.h"
#include "Fire.h"

typedef struct GameStruct {
    std::map<int, TileStruct> tiles;
    int currentPlayer;
    std::map<int, PlayerStruct> players;
    std::map<int, WorldObjectStruct> worldObjects;
    stateType state;
    int boardSize;
    BoardStruct board;
} GameStruct;

typedef struct WorldObjectListStruct {
    std::map<int, WorldObjectStruct> worldObjects;
} WorldObjectListStruct;

class Game
{
private:    
    std::map<int, Tile> tiles;
    int tileId;
    uint16_t effectId;
    uint16_t objectId;
    int currentPlayer;
    std::map<int, Player> players;
    std::map<int, WorldObject> worldObjects;
    std::map<int, Fire> effects;
    std::unordered_set<int> tilesWithDelta;
    std::unordered_set<int> wosWithDelta;
    stateType state;
    int boardSize;
    Board board;
    bool isNextPlayerTurn {false};
    bool newLevel;
    unsigned seed;
    std::default_random_engine generator;
    std::vector<castedSpellData> castedSpells;

public:
    Game(unsigned seed);
    ~Game();

    void createPlayer(std::string name);
    void addPlayer(Player player);
    void removeAllPlayers();
    void update(gameInput input, int playerId, double dt);
    std::map<int, Tile>& getTiles();
    Tile& getTile(TileIdx tileIdx);
    std::map<int, Player>& getPlayers();
    std::map<int, WorldObject>& getWorldObjects();
    WorldObject& getWorldObject(int id);
    void setWorldObjectData(WorldObjectStruct& worldobject);
    Player& getPlayer(int id);
    void addTile(Tile tile, TileIdx tileIdx);
    void removeAllTiles();
    void addWorldObject(WorldObject& worldObject);
    void checkPlayersInVoid();
    bool tryMove(Player& player, double dt);
    bool tryMove(WorldObject& worldObject, double dt);
    bool moveAim(Player& player, double dt);
    bool isWorldObjectPlayer(WorldObject& worldObject, int& playerId);
    void executeProperties(WorldObject& worldObject);
    void killPlayer(Player& player);
    void moveToSpawn(Player& player);
    std::vector<castedSpellData>& getCastedSpells();
    int getNewSpellId();
    void castSpell();
    void updatePosOfWorldObjects(double dt);
    void updateCastedSpells();
    void executeSpell(castedSpellData& spell);
    void affectWorldObject(castedSpellData& spell);
    void checkWinCondition();
    std::map<int,TileIdx> getTextBlockTiles();
    void removeBlocksProperties();
    void nextPlayer();
    void checkTurnTime();
    //stateType getState();
    //void setState(stateType stateT);
    //void storeState();
    bool isOutOfBounds(TileIdx tileIdx);
    int getNewTileId();
    uint16_t getNewEffectId();
    uint16_t getNewObjectId();
    void addAssociateObject(Tile& tile, TileIdx tileIdx);
    void generateLevel();
    bool isASpawnTile(TileIdx tileIdx);
    void setPlayersSpawnTiles();
    void addEffect();
    // Network
    void getAllData(GameStruct& m) const;
    void setAllData(GameStruct& m);
    void getDeltaData(GameStruct& m) const;
    void setDeltaData(GameStruct& m);
};

#endif