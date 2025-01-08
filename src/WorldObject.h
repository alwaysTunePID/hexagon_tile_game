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
    uint16_t width;
    uint16_t height;
} WorldObjectStruct;

class WorldObject
{
private:
    uint16_t id;
    WorldObjectType type;
    worldPos pos;
    worldPos vel;
    worldPos acc;
    directionType dir;
    uint16_t width;
    uint16_t height;
    bool moving;
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
    worldPos getUpdatedPos(double dt);
    bool isMoving();
    void setVelocity(moveInput& move);
    void getAllData(WorldObjectStruct& m) const;
    void setAllData(WorldObjectStruct& m);
};

#endif