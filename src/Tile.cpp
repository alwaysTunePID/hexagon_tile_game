#include <iostream>
#include <stdexcept>
#include <unordered_set>
#include "Tile.h"

Tile::Tile()
    : id{}, tileIdx{}, dir{}, type{}, effect{}, active{}, highlighted{}, properties{}, deltas{}
{}

Tile::Tile(tileType tileType, int idt, std::map<uint8_t, GameDeltas>* deltas)
    : id{ idt }, tileIdx{ 0, 0 }, dir{}, type{ tileType }, effect{}, active{ false }, highlighted{ false }, properties{}, deltas{ deltas }
{
    switch (GetTextureType(type))
    {
    case textureType::directional:
        dir = directionType::up;
        break;

    default:
        dir = directionType::none;
        break;
    }

    properties.push_back({GetInheritEffect(type), directionType::none});
    for (auto& [playerId, delta] : *deltas)
        delta.tilesWithDelta.insert(id);
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
    for (auto& [playerId, delta] : *deltas)
        delta.tilesWithDelta.insert(id);
}

directionType Tile::getDirection()
{
    return dir;
}

void Tile::setDirection(directionType direction)
{
    dir = direction;
    for (auto& [playerId, delta] : *deltas)
        delta.tilesWithDelta.insert(id);
}

bool Tile::isActive()
{
    return active;
}

void Tile::setActive(bool activeT)
{
    active = activeT;
    for (auto& [playerId, delta] : *deltas)
        delta.tilesWithDelta.insert(id);
}

bool Tile::isHighlighted()
{
    return highlighted;
}

void Tile::setHighlighted(bool highlightedT)
{
    highlighted = highlightedT;
    for (auto& [playerId, delta] : *deltas)
        delta.tilesWithDelta.insert(id);
}

tileType Tile::getTileType()
{
    return type;
}

void Tile::setType(tileType newType)
{
    textureType prevTextureType {GetTextureType(type)};
    textureType newTextureType {GetTextureType(newType)};

    type = newType;
    clearProperties();
    addEffect({GetInheritEffect(newType), directionType::none, false});

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

    for (auto& [playerId, delta] : *deltas)
        delta.tilesWithDelta.insert(id);
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
            for (auto& [playerId, delta] : *deltas)
                delta.tilesWithDelta.insert(id);
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
            for (auto& [playerId, delta] : *deltas)
                delta.tilesWithDelta.insert(id);
            return;
        }
    }
}

void Tile::addEffect(effectData effect)
{
    properties.push_back(effect);
    for (auto& [playerId, delta] : *deltas)
        delta.tilesWithDelta.insert(id);
}

std::vector<effectData>& Tile::getProperties()
{
    return properties;
}

void Tile::clearProperties()
{
    properties.clear();
    for (auto& [playerId, delta] : *deltas)
        delta.tilesWithDelta.insert(id);
}

// Network
void Tile::getAllData(TileStruct& m) const
{
    m.id          = id;
    m.tileIdx     = tileIdx;
    m.dir         = dir;
    m.type        = type;
    m.effect      = effect;
    m.active      = active;
    m.highlighted = highlighted;
}

void Tile::setAllData(TileStruct& m)
{
    id          = m.id;
    tileIdx     = m.tileIdx;
    dir         = m.dir;
    type        = m.type;
    effect      = m.effect;
    active      = m.active;
    highlighted = m.highlighted;
}