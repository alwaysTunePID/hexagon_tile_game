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
    std::vector<int> wosToDelete;
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
    std::map<uint8_t, GameDeltas> deltas;
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

    void createPlayer(uint16_t id, std::string name);
    void update(gameInput input, int playerId, double dt, uint8_t& soundState);
    std::map<int, Tile>& getTiles();
    Tile& getTile(TileIdx tileIdx);
    std::map<int, Player>& getPlayers();
    std::map<int, WorldObject>& getWorldObjects();
    WorldObject& getWorldObject(int id);
    void setWorldObjectData(WorldObjectStruct& worldobject);
    Player& getPlayer(int id);
    int getCurrentPlayerId();
    void addTile(Tile tile, TileIdx tileIdx);
    void removeAllTiles();
    void addWorldObject(WorldObject& worldObject);
    void deleteWorldObject(int worldObjectId);
    void checkPlayersInVoid();
    bool tryMove(WorldObject& worldObject, double dt);
    bool isWorldObjectPlayer(WorldObject& worldObject, int& playerId);
    int getPlayerIdToWOId(uint16_t worldObjectId);
    void executeProperties(WorldObject& worldObject);
    void killPlayer(Player& player);
    void moveToSpawn(Player& player);
    std::vector<castedSpellData>& getCastedSpells();
    int getNewSpellId();
    void castSpell(uint8_t& soundState);
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
    void clearDeltaData(uint8_t playerId);
    void getDeltaData(GameStruct& m, uint8_t playerId) const;
    void setDeltaData(GameStruct& m);
};

#endif