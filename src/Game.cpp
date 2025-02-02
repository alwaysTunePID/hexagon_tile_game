#include <iostream>
#include <chrono>
#include <iostream>
#include <unordered_set>
#include "Enums.h"
#include "Transformations.h"
#include "Game.h"
#include "Player.h"
#include "Tile.h"
#include "Board.h"

Game::Game(unsigned seed)
    : tileId{ 0 }, effectId { 0 }, objectId { 0 }, players{}, worldObjects{}, effects{}, deltas{}, boardSize { 8 }, board{ boardSize },
      state { stateType::lobby }, seed{seed}
{
    castedSpellData castedSpell{};
    currentPlayer = 0;
    newLevel = true;
    generator = std::default_random_engine{seed};
}

Game::~Game()
{
}

void Game::createPlayer(uint16_t id, std::string name)
{
    uint16_t newCharacterId{ getNewObjectId() };
    uint16_t newAimId{ getNewObjectId() };
    TileIdx tileIdx{ board.getInitTileIdxFromPlayerId(id) };
    //tilesWithDelta{}, wosWithDelta{}, wosToDelete{}
    deltas.insert({ (uint8_t)id, GameDeltas{{},{},{}} });

    WorldObject worldObject{ newCharacterId, WorldObjectType::player, TileIdxToWorldPos(tileIdx), &deltas };
    WorldObject aimWorldObject{ newAimId,    WorldObjectType::visual, TileIdxToWorldPos(tileIdx), &deltas };
    addWorldObject(worldObject);
    addWorldObject(aimWorldObject);

    Player player{id, newCharacterId, newAimId, name, tileIdx, &worldObjects };
    if (id == 0)
        player.setCurrentPlayer(true);
    players.insert({id, player});
}

void Game::update(gameInput input, int playerId, double dt)
{
    //TODO: Temp hardcoding!
    //playerId = currentPlayer;
    //std::vector<Tile> playerBlocks {};
    //getPlayerBlocks(playerBlocks);
    directionType moveDirection{ GetDirectionType(input.action) };
    bool actionTaken{ false };

    isNextPlayerTurn = (input.action == actionType::skipTurn);

    Player& player{ players[playerId] };

    moveInput zeroVel{ 0.f, 0 };

    switch (state)
    {
    case stateType::playerTurn:

        //if (playerId != currentPlayer)
        //    return;

        // TODO: Optimize this!!
        for (auto& [id, tile] : tiles)
            tile.setHighlighted(false);

        if (player.isLJoyMode(LJoyMode::move))
        {
            bool moved{ false };
            // moved = tryMove(player, dt);
            //if (moved)
            //{
            //    executeProperties(player);
            //}
            player.setVelocity(input.move);
            player.setAimVelocity(zeroVel);
        }
        else if (player.isLJoyMode(LJoyMode::aim))
        {
            player.setVelocity(zeroVel);
            player.setAimVelocity(input.move);
        }

        switch (input.button)
        {
        case xbox::X:
            player.toggleLJoyMode();
            player.getAimWO().setPos(player.getWorldPos());
            break;

        case xbox::A:
            if (player.isLJoyMode(LJoyMode::aim))
            {
                if (playerId == currentPlayer)
                {
                    player.toggleLJoyMode();
                    castSpell();
                }
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

        updatePosOfWorldObjects(dt);
        checkPlayersInVoid();

        // TODO: Move to a function
        if (player.isLJoyMode(LJoyMode::aim) && player.isCurrentPlayer() )
        {
            int toTileId{ board.getTileId(player.getAimWO().getTileIdx()) };
            tiles.at(toTileId).setHighlighted(true);
        }


        //updateCastedSpells();

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

std::map<int, WorldObject>& Game::getWorldObjects()
{
    return worldObjects;
}

WorldObject& Game::getWorldObject(int id)
{
    return worldObjects[id];
}

void Game::setWorldObjectData(WorldObjectStruct& worldobject)
{
    worldObjects[worldobject.id].setAllData(worldobject);
}

Player& Game::getPlayer(int id)
{
    return players[id];
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

void Game::addWorldObject(WorldObject& worldObject)
{
    worldObjects.insert({ worldObject.getId(), worldObject });
}

// Can't be called while iterating through the worldObjects map
void Game::deleteWorldObject(int worldObjectId)
{
    for (auto& [playerId, delta] : deltas)
    {
        delta.wosToDelete.insert(worldObjectId);
        delta.wosWithDelta.erase(worldObjectId);
    }

    if (worldObjects.erase(worldObjectId) == (size_t)0 )
    {
        std::cout << "ERROR: Tried to erase non-existing WorldObject with id: " << worldObjectId << std::endl;
    }
}

void Game::checkPlayersInVoid()
{
    for (auto& [id, player] : players)
    // Delete objects that has fallen to the void
    if (player.getWorldPos().z < -10.0)
    {
        killPlayer(player);
    }
}

bool Game::tryMove(WorldObject& worldObject, double dt)
{
    worldPos pos{ worldObject.getUpdatedPos(dt) };
    TileIdx newTileIdx{ WorldPosToTileIdx(pos) };

    if (newTileIdx == worldObject.getTileIdx())
    {
        // TODO: Optimize, WorldPosToTileIdx is called twice now. Here and in setPos.
        worldObject.setPos(pos);
        return false;
    }

    else if (board.isOutOfBounds(newTileIdx))
    {
        worldObject.setPos(pos);
        if (worldObject.canFall())
        {
            worldObject.canTakeInput(false);
            worldPos vel{ 0.0, 0.0, -10.0 };
            worldObject.setVelocity(vel);
        }
        return false;
    }
    //else if (tiles[board.getTileId(newTileIdx)].hasEffect(EffectType::stop))
    //{
    //    return false;
    //}
    else
    {
        worldObject.setPos(pos);
        return true;
    }
}

bool Game::isWorldObjectPlayer(WorldObject& worldObject, int& playerId)
{
    for (auto& [id, player] : players)
    {
        int worldObjectId{ player.getWorldObjectIds() };
        if (worldObjectId == worldObject.getId())
        {
            playerId = id;
            return true;
        }
    }
    return false;
}

int Game::getPlayerIdToWOId(uint16_t woId)
{
    for (auto& [id, player] : players)
    {
        if (player.getWorldObjectIds() == woId)
            return id;
    }
    return -1;
}

void Game::executeProperties(WorldObject& worldObject)
{
    for (int tileId : board.getTileIds(worldObject.getTileIdx()))
    {
        for (effectData effect : tiles[tileId].getProperties())
        {
            int playerId;
            moveInput push{ 1.f , 100 };
            switch (effect.type)
            {
            case EffectType::sink:
                if (isWorldObjectPlayer(worldObject, playerId))
                    killPlayer(players.at(playerId));
                // Need an else here where the object gets deleted
                break;

            case EffectType::wind:
                // Fix this hardcoded shit!
                worldObject.setVelocity(push);
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
    //tiles[player.getSpawnTileId()].activateEffect(EffectType::spawn);
    player.setWorldPos(TileIdxToWorldPos(tiles[player.getSpawnTileId()].getTileIdx()));
    player.canTakeInput(true);
    player.setVelocity(moveInput{ 0.f, 0.f });
}

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

    worldPos fromPos{ player.getWorldPos() };
    worldPos destination{ TileIdxToWorldPos(player.getAimWO().getTileIdx()) };
    // TODO: fix hardcoded
    float speed{ 1.f };
    worldPos vel{ CalcWorldVelVec(fromPos, destination, speed) };

    uint16_t id{ getNewObjectId() };
    WorldObject fireball{ id, WorldObjectType::fireball, fromPos, &deltas };
    fireball.setVelocity(vel);
    fireball.setPos(fireball.getUpdatedPos(0.3));
    addWorldObject(fireball);

    // Graphics need to know how to draw player here
    /*
    if (spellType == SpellType::teleport)
        player.setSpellOngoing(true);

    castedSpellData castedSpell{};
    castedSpell.id = getNewSpellId();
    castedSpell.playerId = player.getId();
    castedSpell.spellType = spellType;
    castedSpell.dir = player.getSelectedSpellDir();
    castedSpell.fromTileIdx = player.getTileIdx();
    castedSpell.toTileIdx = player.getAimWO().getTileIdx();

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
    }*/
}

void Game::updatePosOfWorldObjects(double dt)
{
    std::vector<int> movedWorldObjects{};
    for (auto& [id, worldObject] : worldObjects)
    {
        if (worldObject.isMoving())
        {
            movedWorldObjects.push_back(id);
            bool moved{ tryMove(worldObject, dt) };
            if (moved)
                executeProperties(worldObject);
        }
    }

    std::vector<int> handledIds{};
    std::vector<int> worldObjectsToRemove{};
    for(int id : movedWorldObjects)
    {
        auto& worldObject{ worldObjects.at(id) };

        for (auto& [id2, worldObject2] : worldObjects)
        {
            if (id2 == id || std::find(handledIds.begin(), handledIds.end(), id2) != handledIds.end())
                continue;

            if (worldObject.isIntersecting(worldObject2))
            {
                if (worldObject.isCollideable() && worldObject2.isCollideable())
                {
                    //std::cout << ToString(worldObject.getType()) << " collided with " << ToString(worldObject2.getType()) << std::endl;
                    Tile& tile{ tiles.at(board.getTileId(worldObject2.getTileIdx())) };
                    tileType newTileType{ GetTileFromCollision(tile.getTileType(), worldObject.getType()) };
                    if (newTileType != tileType::last)
                        tile.setType(newTileType);

                    collisionOutcome collOutcome{ GetWorldObjectsFromCollision(worldObject.getType(), worldObject2.getType()) };
                    if (collOutcome.movedWO == WorldObjectType::remove)
                        worldObjectsToRemove.push_back(id);
                    else if (collOutcome.movedWO != WorldObjectType::last)
                        worldObject.setType(collOutcome.movedWO);

                    if (collOutcome.worldObject == WorldObjectType::remove)
                        worldObjectsToRemove.push_back(id2);
                    else if (collOutcome.worldObject != WorldObjectType::last)
                        worldObject2.setType(collOutcome.worldObject);
                    moveInput zeroVel{ 0.f, 0 };
                    worldObject.setVelocity(zeroVel);
                }
            }
        }
        handledIds.push_back(id);
    }

    for (int id : worldObjectsToRemove)
        deleteWorldObject(id);
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
        tile.setType(outcome.tile);

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

    affectWorldObject(castedSpell);
}

void Game::affectWorldObject(castedSpellData& spell)
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
{}*/

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

uint16_t Game::getNewObjectId()
{
    uint16_t returnId = objectId;
    objectId++;
    return returnId;
}

void Game::addAssociateObject(Tile& tile, TileIdx tileIdx)
{
    // switch didn't work because of stupid initialization of class in switch case issue.
    if (tile.getTileType() == tileType::mountain)
    {
        uint16_t id{ getNewObjectId() };
        WorldObject mountain{ id, WorldObjectType::mountain, TileIdxToWorldPos(tileIdx), &deltas };
        addWorldObject(mountain);
    }
}

void Game::generateLevel()
{
    std::array<int, 3> OBJECT_PROB{ 60, 30, 10 };  // { 30, 30, 40 };
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
            Tile tile{ type, getNewTileId(), &deltas };

            if (isASpawnTile(tileIdx))
            {
                // TODO: Hardcoded
                tile.setType(tileType::grass);
                tile.addEffect({EffectType::spawn, directionType::none, true});
            }

            addTile(tile, tileIdx);
            addAssociateObject(tile, tileIdx);
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

// Network
void Game::getAllData(GameStruct& m) const
{
    for (auto& [id, tile] : tiles)
    {
        TileStruct tileS;
        tile.getAllData(tileS);
        m.tiles.insert({id, tileS});
    }

    m.currentPlayer  = currentPlayer;

    for (auto& [id, player] : players)
    {
        PlayerStruct playerS;
        player.getAllData(playerS);
        m.players.insert({id, playerS});
    }

    for (auto& [id, worldObj] : worldObjects)
    {
        WorldObjectStruct worldObjectS;
        worldObj.getAllData(worldObjectS);
        m.worldObjects.insert({id, worldObjectS});
    }

    m.state = state;
    m.boardSize = boardSize;
    board.getAllData(m.board);
}

void Game::setAllData(GameStruct& m)
{
    tiles.clear();
    for (auto& [id, tileS] : m.tiles)
    {
        Tile tile;
        tile.setAllData(tileS);
        tiles.insert({id, tile});
    }

    currentPlayer  = m.currentPlayer;

    players.clear();
    for (auto& [id, playerS] : m.players)
    {
        Player player;
        player.setAllData(playerS);
        players.insert({id, player});
    }

    worldObjects.clear();
    for (auto& [id, worldObjectS] : m.worldObjects)
    {
        WorldObject worldObj;
        worldObj.setAllData(worldObjectS);
        worldObjects.insert({id, worldObj});
    }

    state = m.state;
    boardSize = m.boardSize;
    board.setAllData(m.board);
}

void Game::clearDeltaData(uint8_t playerId)
{
    deltas.at(playerId).tilesWithDelta.clear();
    deltas.at(playerId).wosWithDelta.clear();
    deltas.at(playerId).wosToDelete.clear();
}

void Game::getDeltaData(GameStruct& m, uint8_t playerId) const
{
    // TODO: Continue to make delta of everything
    for (auto id : deltas.at(playerId).tilesWithDelta)
    {
        TileStruct tileS;
        tiles.at(id).getAllData(tileS);
        m.tiles.insert({id, tileS});
    }

    m.currentPlayer  = currentPlayer;

    for (auto& [id, player] : players)
    {
        PlayerStruct playerS;
        player.getAllData(playerS);
        m.players.insert({id, playerS});
    }

    for (auto id : deltas.at(playerId).wosWithDelta)
    {
        WorldObjectStruct worldObjectS;
        worldObjects.at(id).getAllData(worldObjectS);
        m.worldObjects.insert({id, worldObjectS});
    }

    for (auto id : deltas.at(playerId).wosToDelete)
        m.wosToDelete.push_back(id);

    m.state = state;
    m.boardSize = boardSize;
    board.getAllData(m.board);
}

void Game::setDeltaData(GameStruct& m)
{
    // TODO: Continue to make delta of everything
    for (auto& [id, tileS] : m.tiles)
    {
        // TODO: Continue to make delta. But for now, replace the element
        tiles.erase(id);
        Tile tile;
        tile.setAllData(tileS);
        tiles.insert({id, tile});
    }

    currentPlayer  = m.currentPlayer;

    players.clear();
    for (auto& [id, playerS] : m.players)
    {
        Player player;
        player.setAllData(playerS);
        players.insert({id, player});
    }

    for (auto& [id, worldObjectS] : m.worldObjects)
    {
        // TODO: Continue to make delta. But for now, replace the element
        worldObjects.erase(id);
        WorldObject worldObj;
        worldObj.setAllData(worldObjectS);
        worldObjects.insert({id, worldObj});
    }

    for (int id : m.wosToDelete)
        worldObjects.erase(id);

    state = m.state;
    boardSize = m.boardSize;
    board.setAllData(m.board);
}