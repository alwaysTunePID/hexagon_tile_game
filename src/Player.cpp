#include <iostream>
#include <stdexcept>
#include "Player.h"
#include "Tile.h"

Player::Player()
{}

Player::Player(int id, TileIdx tileIdx)
    : points{ 0 }, turnTime{ 40 }, name{ "Bob" }, 
      id{ id }, tileIdx{ tileIdx }, spawnTileId{}, deathCounter{ 0 }, aimTileId{ 0 }, aimTileIdx{}, 
      clock{}, currentPlayer{ false }, lJoyMode{ LJoyMode::move },
      spellOngoing{ false }, selectedSpellIdx{ 0 },
      selectedSpellDirection{ directionType::none },
      selectionSpells{}, discoveredSpells{}
{
    //TODO: This is not right!!
    selectionSpells.push_back(SpellType::fireball);
    selectionSpells.push_back(SpellType::wind);
    selectionSpells.push_back(SpellType::teleport);
}

Player::Player(int points, int turnTime, int id, bool currentPlayer)
    : points{points}, turnTime{turnTime}, id{id},
      clock{}, currentPlayer{currentPlayer}, lJoyMode{ LJoyMode::move },
      spellOngoing{ false }, selectedSpellIdx{ 0 },
      selectedSpellDirection{ directionType::none },
      selectionSpells{}, discoveredSpells{}
{}

Player::~Player()
{}

void Player::addPoint()
{
    points++;
}

int Player::getPoints()
{
    return points;
}

std::string Player::getName()
{
    return name;
}

int Player::getId()
{
    return id;
}

TileIdx& Player::getTileIdx()
{
    return tileIdx;
}

void Player::setTileIdx(TileIdx tileIdxt)
{
    tileIdx = tileIdxt;
}

int Player::getSpawnTileId()
{
    return spawnTileId;
}

void Player::setSpawnTileId(int tileIdt)
{
    spawnTileId = tileIdt;
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
    deathCounter += 1;
}