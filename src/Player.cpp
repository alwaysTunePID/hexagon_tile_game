#include <iostream>
#include <stdexcept>
#include <cmath>
#include "Player.h"
#include "Tile.h"
#include "WorldObject.h"
#include "Transformations.h"

Player::Player()
{}

Player::Player(int id, std::string name, TileIdx tileIdx, std::map<int, WorldObject>* worldObjects)
    : turnTime{ 40 }, name{ name }, 
      id{ id }, stats{}, tileIdx{ tileIdx }, pos{0.f, 0.f}, vel{0.f, 0.f}, w_pos{ TileIdxToWorldPos(tileIdx) }, 
      w_vel{0.f, 0.f, 0.f}, dir{ directionType::up }, spawnTileId{},
      aimTileId{ 0 }, aimTileIdx{}, aimPos{0.f, 0.f}, aimVel{0.f, 0.f}, 
      clock{}, currentPlayer{ false }, lJoyMode{ LJoyMode::move },
      spellOngoing{ false }, selectedSpellIdx{ 0 },
      selectedSpellDirection{ directionType::none },
      selectionSpells{}, discoveredSpells{}, worldObjects{ worldObjects }
{
    //TODO: This is not right!!
    selectionSpells.push_back(SpellType::fireball);
    selectionSpells.push_back(SpellType::wind);
    selectionSpells.push_back(SpellType::teleport);

    //std::cout << "World pos player " << id << ":" << w_pos.x << " " << w_pos.y << std::endl; 
}

Player::Player(playerStats stats, int turnTime, int id, bool currentPlayer)
    : stats{stats}, turnTime{turnTime}, id{id},
      clock{}, currentPlayer{currentPlayer}, lJoyMode{ LJoyMode::move },
      spellOngoing{ false }, selectedSpellIdx{ 0 },
      selectedSpellDirection{ directionType::none },
      selectionSpells{}, discoveredSpells{}
{}

Player::~Player()
{}

std::string Player::getName()
{
    return name;
}

void Player::setName(std::string newName)
{
    name = newName;
}

int Player::getId()
{
    return id;
}

playerStats Player::getStats()
{
    return stats;
}

TileIdx& Player::getTileIdx()
{
    return tileIdx;
}

void Player::setTileIdx(TileIdx tileIdxt)
{
    tileIdx = tileIdxt;
}

PosInTile& Player::getPos()
{
    return pos;
}

void Player::setPos(PosInTile posT)
{
    pos = posT;
}

worldPos Player::getWorldPos()
{
    return (*worldObjects)[id].getPos(); // DO THIS FOR ALL THE POS STUFF! 
}

void Player::setWorldPos(worldPos posT)
{
    (*worldObjects)[id].setPos(posT);
}

PosInTile& Player::getAimPos()
{
    return aimPos;
}

void Player::setAimPos(PosInTile posT)
{
    aimPos = posT;
}

directionType Player::getDir()
{
    return (*worldObjects)[id].getDir();;
}

// TODO: This should return a vector of ids. 
int Player::getWorldObjectIds()
{
    return id;
}

int Player::getSpawnTileId()
{
    return spawnTileId;
}

void Player::setSpawnTileId(int tileIdt)
{
    spawnTileId = tileIdt;
}

worldPos Player::getUpdatedPos(double dt)
{
    return (*worldObjects)[id].getUpdatedPos(dt);
}

PosInTile Player::getUpdatedAimPos(double dt)
{
    PosInTile updatedPos{
        aimPos.first + aimVel.first * static_cast<float>(dt),
        aimPos.second + aimVel.second * static_cast<float>(dt)
    };
    return updatedPos;
}

void Player::canTakeInput(bool can)
{
    (*worldObjects)[id].canTakeInput(can);
}

void Player::setVelocity(moveInput& move)
{
    (*worldObjects)[id].setVelocity(move);
}

void Player::setAimVelocity(moveInput& move)
{
    aimVel.first = move.power * std::cos(move.angle);
    aimVel.second = move.power * std::sin(move.angle);
}

bool Player::isCurrentPlayer()
{
    return currentPlayer;
}

void Player::setCurrentPlayer(bool isCurrent)
{
    currentPlayer = isCurrent;
    clock.restart();
}

void Player::updateTurnTime()
{
    // Can we change this??
    float maxTurnTime{ static_cast<float>(TURN_TIME) };

    if (currentPlayer)
    {
        float lapTime = clock.getElapsedTime().asSeconds();
        turnTime = (maxTurnTime - lapTime);
    }
    else
    {
        turnTime = maxTurnTime;
    }
}

float Player::getTurnTime()
{
    return turnTime;
}

bool Player::isLJoyMode(LJoyMode mode)
{
    return lJoyMode == mode;
}

void Player::toggleLJoyMode()
{
    lJoyMode = (lJoyMode == LJoyMode::move) ? LJoyMode::aim : LJoyMode::move;

    if (lJoyMode == LJoyMode::aim)
    {
        selectedSpellDirection = IsDirectional(getSelectedSpell()) ? directionType::up : directionType::none;
    }
}

int Player::getAimTileId()
{
    return aimTileId;
}

void Player::setAimTile(Tile& tile)
{
    aimTileId = tile.getId();
    aimTileIdx = tile.getTileIdx();
}

TileIdx Player::getAimTileIdx()
{
    return aimTileIdx;
}

SpellType Player::getSelectedSpell()
{
    return selectionSpells.at(selectedSpellIdx);
}

int Player::getSelectedSpellIdx()
{
    return selectedSpellIdx;
}

void Player::selectNextSpell()
{
    selectedSpellIdx++;
    if (selectedSpellIdx == selectionSpells.size())
        selectedSpellIdx = 0;

    selectedSpellDirection = IsDirectional(getSelectedSpell()) ? directionType::up : directionType::none;
}

directionType Player::getSelectedSpellDir()
{
    return selectedSpellDirection;
}

void Player::setSelectedSpellDir(directionType direction)
{
    selectedSpellDirection = direction;
}

void Player::rotateSpellDirection()
{
    selectedSpellDirection = RotateDirection(selectedSpellDirection);
}

bool Player::isSpellOngoing()
{
    return spellOngoing;
}

void Player::setSpellOngoing(bool isOngoing)
{
    spellOngoing = isOngoing;
}

std::vector<SpellType>& Player::getSelectionSpells()
{
    return selectionSpells;
}

void Player::increaseDeathCounter()
{
    stats.deaths += 1;
}

// Network
void Player::getAllData(PlayerStruct& m) const
{
    m.id       = id;
    m.stats    = stats;
    m.turnTime = turnTime;
    m.name     = name;
}

void Player::setAllData(PlayerStruct& m)
{
    id       = m.id;
    stats    = m.stats;
    turnTime = m.turnTime;
    name     = m.name;
}