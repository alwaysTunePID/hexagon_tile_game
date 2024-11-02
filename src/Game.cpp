#include <iostream>
#include <chrono>
#include <iostream>
#include "Enums.h"
#include "Game.h"
#include "Player.h"
#include "Tile.h"
#include "Board.h"

Game::Game(unsigned seed)
    : tileId{ 0 }, effectId { 0 }, players{}, effects{}, boardSize { 8 }, board{ boardSize },
      state { stateType::lobby }, seed{seed}, updateCounter{}
{
    castedSpellData castedSpell{};
    currentPlayer = 0;
    newLevel = true;
    generator = std::default_random_engine{seed};
}

Game::~Game()
{
}

void Game::createPlayer()
{
    static int newPlayerId{ 0 };
    TileIdx tileIdx{ board.getInitTileIdxFromPlayerId(newPlayerId) };
    Player player{newPlayerId, tileIdx};
    if (newPlayerId == 0)
        player.setCurrentPlayer(true);
    players.insert({newPlayerId, player});

    newPlayerId++;
}

void Game::addPlayer(Player player)
{
    players.insert({player.getId(), player});
}


void Game::removeAllPlayers()
{
    players.clear();
}


void Game::update(gameInput input, int playerId, double dt)
{
    updateCounter++;
    //TODO: Temp hardcoding!
    playerId = currentPlayer;
    bool printCurrentRules {false};
    //std::vector<Tile> playerBlocks {};
    //getPlayerBlocks(playerBlocks);
    directionType moveDirection{ GetDirectionType(input.action) };
    bool actionTaken{ false };

    isNextPlayerTurn = (input.action == actionType::skipTurn);

    std::vector<std::vector<int>> textLines{};
    std::vector<EffectType> textBlocks{};

    Player& player{ players[playerId] };

    moveInput zeroVel{ 0.f, 0 };

    switch (state)
    {
    case stateType::playerTurn:

        if (playerId != currentPlayer)
            return;

        if (player.isLJoyMode(LJoyMode::move))
        {
            bool moved{ false };
            moved = tryMove(player, dt);
            if (moved)
            {
                executeProperties(player);
            }
            player.setVelocity(input.move);
        }

        if (player.isLJoyMode(LJoyMode::aim))
        {
            //TileIdx fromTile{ player.getAimTileIdx() };
            //TileIdx toTile{ board.getTileInFront(fromTile, moveDirection) };
            moveAim(player, dt);
            player.setAimVelocity(input.move);
        }

        switch (input.button)
        {
        case xbox::X:
            player.toggleLJoyMode();
            tiles[player.getAimTileId()].setHighlighted(false);

            if (player.isLJoyMode(LJoyMode::aim))
            {
                int tileId{ board.getTileId(player.getTileIdx()) };
                player.setAimTile(tiles[tileId]);
                player.setVelocity(zeroVel);
            }
            else
            {
                player.setAimVelocity(zeroVel);
            }
            break;

        case xbox::A:
            if (player.isLJoyMode(LJoyMode::aim))
            {
                player.toggleLJoyMode();
                tiles[player.getAimTileId()].setHighlighted(false);
                castSpell();
            }
            break; 

        case xbox::LB:
        if (player.isLJoyMode(LJoyMode::aim))
                player.selectNextSpell();
            break;

        case xbox::RB:
            if (player.isLJoyMode(LJoyMode::aim))
                player.rotateSpellDirection();
            break;
        
        default:
            break;
        }

        /*else if (input.action == actionType::undoMove)
        {
            if (blockHistory.size() > 1)
            {
                // This initial pop looks strange but it is because we pre-store states
                blockHistory.pop_back();
                std::pair<std::map<int, Tile>, Board> blockState{ blockHistory.back() };
                blocks = blockState.first;
                board = blockState.second;
                blockHistory.pop_back();
                actionTaken = true;
            }
        }*/

        updateCastedSpells();

        if (actionTaken || newLevel)
        {
            newLevel = false;

            //checkWinCondition();
        }

        checkTurnTime();

        if (isNextPlayerTurn)
        {
            nextPlayer();
        }

        for (auto& [id, player] : players)
        {
            player.updateTurnTime();
        }

        // Debug stuff
        /*
        if (updateCounter % 20u == 0)
        {
            TileIdx playerTileIdx{ WorldPosToTileIdx(player.getWorldPos()) };
            std::cout << "playerTileIdx: " << playerTileIdx.first << " " << playerTileIdx.second << std::endl;
        }*/
        break;

    case stateType::lobby:
        if (players.size() == 2)
        {
            state = stateType::generateLevel;
        }
        break;

    case stateType::generateLevel:
        removeAllTiles();
        generateLevel();
        setPlayersSpawnTiles();

        newLevel = true;

        state = stateType::playerTurn;
        break;

    default:
        break;
    }
}

std::map<int, Tile>& Game::getTiles()
{
    return tiles;
}

Tile& Game::getTile(TileIdx tileIdx)
{
    return tiles[board.getTileId(tileIdx)];
}

std::map<int, Player>& Game::getPlayers()
{
    return players;
}

Player& Game::getPlayer(int id)
{
    return players[id];
}

gameEventType Game::getEvent()
{
    return event;
}

void Game::setEvent(gameEventType eventT)
{
    event = eventT;
}

void Game::addTile(Tile tile, TileIdx tileIdx)
{
    board.addTile(tile, tileIdx);
    tiles.insert({tile.getId(), tile});
}

void Game::removeAllTiles()
{
    for (auto& [id, tile] : tiles)
    {
        board.removeTile(tile);
    }
    tiles.clear();
}

bool Game::tryMove(Player& player, double dt)
{
    worldPos pos{ player.getUpdatedPos(dt) };
    TileIdx newTileIdx{ WorldPosToTileIdx(pos) };

    if (newTileIdx == player.getTileIdx())
    {
        player.setWorldPos(pos);
        return false;
    }

    else if (board.isOutOfBounds(newTileIdx))
    {
        return false;
    }
    else if (tiles[board.getTileId(newTileIdx)].hasEffect(EffectType::stop))
    {
        return false;
    }
    else
    {
        player.setTileIdx(newTileIdx);
        player.setWorldPos(pos);
        return true;
    }
}

bool Game::moveAim(Player& player, double dt)
{
    //PosInTile pos{ player.getUpdatedAimPos(dt) };
    // Temp to be able to compile
    worldPos pos{ player.getUpdatedPos(dt) };
    directionType direction{ board.getNewTileDir(pos) };
    //player.setAimPos(pos);

    if (direction == directionType::none)
    {
        return false;
    }

    TileIdx toTile{ board.getTileInFront(player.getAimTileIdx(), direction) };
    if (board.isOutOfBounds(toTile))
    {
        return false;
    }

    int toTileId{ board.getTileId(toTile) };
    tiles[toTileId].setHighlighted(true);
    tiles[player.getAimTileId()].setHighlighted(false);
    player.setAimTile(tiles[toTileId]);
    return true;
}

void Game::executeProperties(Player& player)
{
    for (int tileId : board.getTileIds(player.getTileIdx()))
    {
        for (effectData effect : tiles[tileId].getProperties())
        {
            moveInput push{ 1.f , 100 };
            switch (effect.type)
            {
            case EffectType::sink:
                killPlayer(player);
                break;

            case EffectType::wind:
                // Fix this hardcoded shit!
                player.setVelocity(push);
                break;

            default:
                break;
            }
        }
    }
}

void Game::killPlayer(Player& player)
{
    player.increaseDeathCounter();
    tiles[player.getSpawnTileId()].activateEffect(EffectType::spawn);
    player.setTileIdx(tiles[player.getSpawnTileId()].getTileIdx());
}

/*
void Game::checkWinCondition()
{
    std::vector<Tile> playerBlocks{};
    getPlayerBlocks(playerBlocks);
    
    for (Tile& player : playerBlocks)
    {
        std::vector<int> blocksOnSameTile{board.getTileIds(player.getTileIdx())};
        for (int blockId : blocksOnSameTile)
        {
            if (tiles[blockId].hasEffect(EffectType::win))
            {
                players[currentPlayer].addPoint();
                // TODO: Is this right?
                nextPlayer();
                // What was the purpose of gameEventType???
                // setEvent(gameEventType::generateLevel);
                state = stateType::generateLevel;
            }
        }
    }
}*/

std::vector<castedSpellData>& Game::getCastedSpells()
{
    return castedSpells;
}

int Game::getNewSpellId()
{
    static int spellId{0};
    int returnVal = spellId;
    spellId++;
    return returnVal;
}

void Game::castSpell()
{
    Player& player{ players[currentPlayer] };
    SpellType spellType{ player.getSelectedSpell() };

    // Graphics need to know how to draw player here
    if (spellType == SpellType::teleport)
        player.setSpellOngoing(true);

    castedSpellData castedSpell{};
    castedSpell.id = getNewSpellId();
    castedSpell.playerId = player.getId();
    castedSpell.spellType = spellType;
    castedSpell.dir = player.getSelectedSpellDir();
    castedSpell.fromTileIdx = player.getTileIdx();
    castedSpell.toTileIdx = tiles[player.getAimTileId()].getTileIdx();

    if (GetSpeed(spellType) < 0)
    {
        executeSpell(castedSpell);
    }
    else
    {
        timePoint startTime{ Time::now() };
        castedSpell.startTime = startTime;
        castedSpell.traveledPerc = 0.0;
        castedSpells.push_back(castedSpell);
    }
}

void Game::updateCastedSpells()
{
    timePoint endTime{ Time::now() };
    for (castedSpellData& castedSpell : castedSpells)
    {
        timeDuration timePast{ endTime - castedSpell.startTime };
        // TODO: Fix this incorrect definition of speed
        castedSpell.traveledPerc = timePast.count() * 100.0 / GetSpeed(castedSpell.spellType);
        // TODO: Check if traveledPerc is done and if so perform its effect
        if (castedSpell.traveledPerc > 100.0)
            executeSpell(castedSpell);
    }

    // Remove finished spells
    castedSpells.erase(std::remove_if(
        castedSpells.begin(), castedSpells.end(),
        [](const castedSpellData& castedSpell) { 
            return castedSpell.traveledPerc > 100.0;
        }), castedSpells.end());
}

void Game::executeSpell(castedSpellData& castedSpell)
{
    Tile& tile{ tiles[board.getTileId(castedSpell.toTileIdx)] };
    std::cout << ToString(tile.getTileType()) << std::endl;
    spellOutcome outcome{ GetTileFromCastSpell(tile.getTileType(), castedSpell.spellType) };
    if (outcome.tile != tileType::last)
        tile.setBlockType(outcome.tile);

    if (outcome.effect != EffectType::last)
    {
        // TODO: Should this be hardcoded?
        if (outcome.effect == EffectType::spawn)
        {
            tile.addEffect({outcome.effect, castedSpell.dir, false});
        }
        else
        {
            tile.addEffect({outcome.effect, castedSpell.dir, true});
        }
    }
        

    std::cout << "Executed Spell dir: " << ToString(castedSpell.dir) << std::endl;

    affectEntity(castedSpell);
}

void Game::affectEntity(castedSpellData& spell)
{
    Player& player{ getPlayer(spell.playerId) };

    switch (spell.spellType)
    {
    case SpellType::teleport:
        player.setTileIdx(spell.toTileIdx);
        player.setSpellOngoing(false);
        break;
    
    default:
        break;
    }
}

void Game::nextPlayer()
{
    players[currentPlayer].setCurrentPlayer(false);
    int numOfPlayers{static_cast<int>(players.size())};
    currentPlayer = (currentPlayer == (numOfPlayers - 1)) ? 0 : currentPlayer + 1;
    players[currentPlayer].setCurrentPlayer(true);
    isNextPlayerTurn = false;
    //blockHistory.clear();
    // TODO: Change name of this!
    newLevel = true;
}

void Game::checkTurnTime()
{
    float turnTimer {players[currentPlayer].getTurnTime()};
    if (turnTimer <= 0)
    {
        isNextPlayerTurn = true;
    }
}

/*
stateType Game::getState()
{
    return state;
}

void Game::setState(stateType stateT)
{
    state = stateT;
}

void Game::storeState()
{
    std::pair<std::map<int, Tile>, Board> blockState{ blocks, board };
    blockHistory.push_back(blockState);
}*/

bool Game::isOutOfBounds(TileIdx tileIdx)
{
    return board.isOutOfBounds(tileIdx);
}

int Game::getNewTileId()
{
    int returnId = tileId;
    tileId++;
    return returnId;
}

uint16_t Game::getNewEffectId()
{
    uint16_t returnId = effectId;
    effectId++;
    return returnId;
}

void Game::generateLevel()
{
    std::array<int, 3> OBJECT_PROB{ 60, 30, 10 };
    std::discrete_distribution<int> dist_objects{ OBJECT_PROB.begin(), OBJECT_PROB.end() };

    for (int i = 0; i <= boardSize; i++)
    {
        int r{ boardSize / 2 - i };

        for (int ii = 0; ii <= boardSize; ii++)
        {
            int c{ boardSize / 2 - ii };

            TileIdx tileIdx{ r, c };
            if (board.isOutOfBounds(tileIdx))
                continue;

            tileType type{ static_cast<tileType>(dist_objects(generator)) };            
            Tile tile(type, getNewTileId());

            if (isASpawnTile(tileIdx))
            {
                // TODO: Hardcoded
                tile.setBlockType(tileType::grass);
                tile.addEffect({EffectType::spawn, directionType::none, true});
            }

            addTile(tile, tileIdx);
        }
    }
}

bool Game::isASpawnTile(TileIdx tileIdx)
{
    for (auto& [id, player] : players)
    {
        if (board.getInitTileIdxFromPlayerId(id) == tileIdx)
        {
            return true;
        }
    }
    return false;
}

void Game::setPlayersSpawnTiles()
{
    for (auto& [id, player] : players)
    {
        int tileId{ board.getTileId(board.getInitTileIdxFromPlayerId(id)) };
        player.setSpawnTileId(tileId);
    }
}

void Game::addEffect()
{
    uint16_t id{ getNewEffectId() };
    //Fire(id, )
}