#ifndef TILE_H
#define TILE_H

#include <SFML/Graphics.hpp>
#include <string>
#include "Enums.h"

class Tile
{
private:
    int id;
    TileIdx tileIdx;
    directionType dir;
    int adjMask;
    int moveIdx;
    tileType type;
    EffectType effect;
    bool active;
    bool highlighted;

    std::vector<effectData> properties;
public:
    Tile();
    Tile(tileType blocktype, int idt);
    ~Tile();

    TileIdx getTileIdx();
    void setTileIdx(TileIdx tilet);
    directionType getDirection();
    void setDirection(directionType direction);
    int getAdjMask();
    void setAdjMask(int adjMaskT);
    int getMoveIdx();
    void setMoveIdx(int moveIdxt);
    bool isActive();
    void setActive(bool activeT);
    bool isHighlighted();
    void setHighlighted(bool highlightedT);
    void increaseMoveIdx();
    tileType getTileType();
    void setBlockType(tileType blocktype);
    EffectType getTextType();
    int getId();
    bool hasEffect(EffectType effect);
    void activateEffect(EffectType effectType);
    void deactivateEffect(EffectType effectType);
    void addEffect(effectData effect);
    std::vector<effectData>& getProperties();
    void clearProperties();

};

#endif