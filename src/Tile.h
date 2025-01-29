#ifndef TILE_H
#define TILE_H

#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_set>
#include "Enums.h"

typedef struct TileStruct {
    int id;
    TileIdx tileIdx;
    directionType dir;
    tileType type;
    EffectType effect;
    bool active;
    bool highlighted;
} TileStruct;

class Tile
{
private:
    int id;
    TileIdx tileIdx;
    directionType dir;
    tileType type;
    EffectType effect;
    bool active;
    bool highlighted;
    std::vector<effectData> properties;
    std::map<sf::Uint8, GameDeltas>* deltas;

public:
    Tile();
    Tile(tileType blocktype, int idt, std::map<sf::Uint8, GameDeltas>* deltas);
    ~Tile();

    TileIdx getTileIdx();
    void setTileIdx(TileIdx tilet);
    directionType getDirection();
    void setDirection(directionType direction);
    bool isActive();
    void setActive(bool activeT);
    bool isHighlighted();
    void setHighlighted(bool highlightedT);
    void increaseMoveIdx();
    tileType getTileType();
    void setType(tileType blocktype);
    EffectType getTextType();
    int getId();
    bool hasEffect(EffectType effect);
    void activateEffect(EffectType effectType);
    void deactivateEffect(EffectType effectType);
    void addEffect(effectData effect);
    std::vector<effectData>& getProperties();
    void clearProperties();
    // Network
    void getAllData(TileStruct& m) const;
    void setAllData(TileStruct& m);

};

#endif