#include <iostream>
#include <stdexcept>
#include <cmath>
#include "Player.h"
#include "Tile.h"
#include "WorldObject.h"
#include "Transformations.h"

Player::Player()
{}

Player::Player(int id, uint16_t characterId, uint16_t aimId, std::string name, TileIdx tileIdx, std::map<int, WorldObject>* worldObjects)
    : turnTime{ 40 }, name{ name }, 
      id{ id }, characterId{ characterId }, stats{}, tileIdx{ tileIdx }, pos{0.f, 0.f}, vel{0.f, 0.f}, w_pos{ TileIdxToWorldPos(tileIdx) }, 
      w_vel{0.f, 0.f, 0.f}, dir{ directionType::up }, spawnTileId{}, aimId{ aimId },
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
    return (*worldObjects)[characterId].getPos(); // DO THIS FOR ALL THE POS STUFF! 
}

void Player::setWorldPos(worldPos posT)
{
    (*worldObjects)[characterId].setPos(posT);
}

directionType Player::getDir()
{
    return (*worldObjects)[characterId].getDir();;
}

// TODO: This should return a vector of ids. 
int Player::getWorldObjectIds()
{
    return characterId;
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
    return (*worldObjects)[characterId].getUpdatedPos(dt);
}

void Player::canTakeInput(bool can)
{
    (*worldObjects)[characterId].canTakeInput(can);
}

void Player::setVelocity(moveInput& move)
{
    (*worldObjects)[characterId].setVelocity(move);
}

void Player::setAimVelocity(moveInput& move)
{
    (*worldObjects)[aimId].setVelocity(move);
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

int Player::getAimId()
{
    return aimId;
}

WorldObject& Player::getAimWO()
{
    return (*worldObjects)[aimId];
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
    m.id          = id;
    m.characterId = characterId;
    m.stats       = stats;
    m.turnTime    = turnTime;
    m.name        = name;
    m.aimId       = aimId;
    m.lJoyMode    = lJoyMode;
    m.selectedSpellIdx       = selectedSpellIdx;
    m.selectedSpellDirection = selectedSpellDirection;
    m.selectionSpells        = selectionSpells;
}

void Player::setAllData(PlayerStruct& m)
{
    id          = m.id;
    characterId = m.characterId;
    stats       = m.stats;
    turnTime    = m.turnTime;
    name        = m.name;
    aimId       = m.aimId;
    lJoyMode    = m.lJoyMode;
    selectedSpellIdx       = m.selectedSpellIdx;
    selectedSpellDirection = m.selectedSpellDirection;
    selectionSpells        = m.selectionSpells;
}