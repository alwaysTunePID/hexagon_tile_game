#ifndef WORLDOBJECT_H
#define WORLDOBJECT_H

#include <vector>
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
    std::vector<Fire> effects;
    std::unordered_set<int>* wosWithDelta;

public:
    WorldObject(uint16_t id, WorldObjectType type,  worldPos w_pos, std::unordered_set<int>* wosWithDelta);
    WorldObject();
    ~WorldObject();

    uint16_t getId();
    WorldObjectType getType();
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
    void setVelocity(worldPos& velIn);
    void setVelocity(moveInput& move);
    void getAllData(WorldObjectStruct& m) const;
    void setAllData(WorldObjectStruct& m);
};

#endif