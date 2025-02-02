#ifndef WORLDOBJECT_H
#define WORLDOBJECT_H

#include <vector>
#include <map>
#include <unordered_set>
#include "Enums.h"
#include "Fire.h"

typedef struct WorldObjectStruct {
    uint16_t id;
    WorldObjectType type;
    worldPos pos;
    worldPos vel;
    worldPos acc;
    directionType dir;
    float width;
    float height;
    sf::Vector2f origin;
} WorldObjectStruct;

class WorldObject
{
private:
    uint16_t id;
    WorldObjectType type;
    worldPos pos;
    worldPos vel;
    worldPos acc;
    TileIdx tileIdx;
    directionType dir;
    float width;
    float height;
    sf::Vector2f origin;
    bool moving;
    bool takeInput;
    bool fallable;
    bool collideable;
    std::vector<Fire> effects;
    std::map<uint8_t, GameDeltas>* deltas;

public:
    WorldObject(uint16_t id, WorldObjectType type,  worldPos w_pos, std::map<uint8_t, GameDeltas>* deltas);
    WorldObject();
    ~WorldObject();

    uint16_t getId();
    WorldObjectType getType();
    void setType(WorldObjectType newType);
    worldPos getPos();
    void setPos(worldPos w_pos);
    directionType getDir();
    float getWidth();
    float getHeight();
    TileIdx getTileIdx();
    worldPos getUpdatedPos(double dt);
    bool isMoving();
    bool isIntersecting(WorldObject& worldObject);
    void canTakeInput(bool can);
    bool canFall();
    bool isCollideable();
    void setVelocity(worldPos& velIn);
    void setVelocity(moveInput& move);
    void getAllData(WorldObjectStruct& m) const;
    void setAllData(WorldObjectStruct& m);
};

#endif