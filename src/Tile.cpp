#include <iostream>
#include <stdexcept>
#include "Tile.h"

Tile::Tile()
{}

Tile::Tile(tileType tileType, int idt)
{
    id = idt;
    moveIdx = 0;
    adjMask = 0;
    active = false;
    highlighted = false;
    type = tileType;

    tileIdx = { 0, 0 };

    switch (GetTextureType(tileType))
    {
    case textureType::directional:
        dir = directionType::up;
        break;

    default:
        dir = directionType::none;
        break;
    }

    properties = std::vector<effectData>{};
    properties.push_back({GetInheritEffect(tileType), directionType::none});
}


Tile::~Tile()
{
}

TileIdx Tile::getTileIdx()
{
    return tileIdx;
}

void Tile::setTileIdx(TileIdx tilet)
{
    tileIdx = tilet;
}

directionType Tile::getDirection()
{
    return dir;
}

void Tile::setDirection(directionType direction)
{
    dir = direction;
}

int Tile::getAdjMask()
{
    return adjMask;
}

void Tile::setAdjMask(int adjMaskT)
{
    adjMask = adjMaskT;
}

int Tile::getMoveIdx()
{
    return moveIdx;
}

void Tile::setMoveIdx(int moveIdxt)
{
    moveIdx = moveIdxt;
}

bool Tile::isActive()
{
    return active;
}

void Tile::setActive(bool activeT)
{
    active = activeT;
}

bool Tile::isHighlighted()
{
    return highlighted;
}

void Tile::setHighlighted(bool highlightedT)
{
    highlighted = highlightedT;
}

void Tile::increaseMoveIdx()
{
    moveIdx = (moveIdx == 3) ? 0 : moveIdx + 1;
}

tileType Tile::getTileType()
{
    return type;
}

void Tile::setBlockType(tileType blocktype)
{
    textureType prevTextureType {GetTextureType(type)};
    textureType newTextureType {GetTextureType(blocktype)};

    type = blocktype;
    clearProperties();
    addEffect({GetInheritEffect(blocktype), directionType::none, false});

    if (prevTextureType == textureType::directional && 
        newTextureType != textureType::directional)
    {
        dir = directionType::none;
    }
    else if (prevTextureType != textureType::directional && 
            newTextureType == textureType::directional)
    {
        dir = directionType::up;
    }
}

int Tile::getId()
{
    return id;
}

bool Tile::hasEffect(EffectType type)
{
    return (std::find_if(properties.begin(), properties.end(),
        [type](effectData effect) { 
            return effect.type == type;
        }) != properties.end());
}

void Tile::activateEffect(EffectType effectType)
{
    for (effectData& effect : properties)
    {
        if (effect.type == effectType)
        {
            effect.active = true;
            return;
        }
    }
}

void Tile::deactivateEffect(EffectType effectType)
{
    for (effectData& effect : properties)
    {
        if (effect.type == effectType)
        {
            effect.active = false;
            return;
        }
    }
}

void Tile::addEffect(effectData effect)
{
    properties.push_back(effect);
}

std::vector<effectData>& Tile::getProperties()
{
    return properties;
}

void Tile::clearProperties()
{
    properties.clear();
}