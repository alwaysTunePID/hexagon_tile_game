#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <chrono>
#include <vector>
#include <map>
#include <random>
#include "Tile.h"
#include "Board.h"
#include "Player.h"

class Game
{
private:
    std::map<int, Tile> tiles;
    int tileId;
    std::vector<std::pair<std::map<int, Tile>,Board>> blockHistory;
    int currentPlayer;
    std::map<int, Player> players;
    stateType state;
    gameEventType event{gameEventType::none};
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

    void createPlayer();
    void addPlayer(Player player);
    void removeAllPlayers();
    void update(gameInput input, int playerId, double dt);
    std::map<int, Tile>& getTiles();
    Tile& getTile(TileIdx tileIdx);
    std::map<int, Player>& getPlayers();
    Player& getPlayer(int id);
    gameEventType getEvent();
    void setEvent(gameEventType event);
    void addTile(Tile tile, TileIdx tileIdx);
    void removeAllTiles();
    bool tryMove(Player& player, double dt);
    bool moveAim(Player& player, double dt);
    void executeProperties(Player& player);
    void killPlayer(Player& player);
    void moveToSpawn(Player& player);
    std::vector<castedSpellData>& getCastedSpells();
    int getNewSpellId();
    void castSpell();
    void updateCastedSpells();
    void executeSpell(castedSpellData& spell);
    void affectEntity(castedSpellData& spell);
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
    void generateLevel();
    bool isASpawnTile(TileIdx tileIdx);
    void setPlayersSpawnTiles();
};

#endif