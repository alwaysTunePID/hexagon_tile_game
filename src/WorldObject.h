#ifndef WORLDOBJECT_H
#define WORLDOBJECT_H

#include <vector>
#include "Enums.h"
#include "Fire.h"

class WorldObject
{
private:
    uint16_t id;
    WorldObjectType type;
    worldPos pos;
    worldVel vel;
    worldAcc acc;
    directionType dir;
    uint16_t width;
    uint16_t height;
    std::vector<Fire> effects;

public:
    WorldObject(uint16_t id, WorldObjectType type,  worldPos w_pos);
    WorldObject();
    ~WorldObject();

    uint16_t getId();
    WorldObjectType getType();
    worldPos getPos();
    void setPos(worldPos w_pos);
    directionType getDir();
    worldPos getUpdatedPos(double dt);
    void setVelocity(moveInput& move);
};

#endif