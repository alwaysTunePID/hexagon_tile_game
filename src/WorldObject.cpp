#include "WorldObject.h"
#include "Transformations.h"

WorldObject::WorldObject(uint16_t id, WorldObjectType type,  worldPos w_pos)
    : id{ id }, type{ type }, pos{ w_pos }, vel{ 0, 0, 0 }, acc{ 0, 0, 0 }, dir{ GetInitDir(type) }, width{}, height{}, effects{}
{}

WorldObject::WorldObject()
    : id{}, type{}, pos{}, vel{}, acc{}, dir{}, width{}, height{}, effects{}
{}

WorldObject::~WorldObject()
{}

uint16_t WorldObject::getId()
{
    return id;
}

WorldObjectType WorldObject::getType()
{
    return type;
}

worldPos WorldObject::getPos()
{
    return pos;
}

void WorldObject::setPos(worldPos w_pos)
{
    pos = w_pos;
}

directionType WorldObject::getDir()
{
    return dir;
}

worldPos WorldObject::getUpdatedPos(double dt)
{
    worldPos updatedPos{
        pos.x + vel.x * dt,
        pos.y + vel.y * dt,
        pos.z + vel.z * dt
    };
    return updatedPos;
}

// Not completely sure if this should be here instead of in Player
void WorldObject::setVelocity(moveInput& move)
{
    vel = InputToWorldVel(move);

    // TODO: Similar code in Board.cpp, move this?
    float ang1{ 0.983 };

    float angle{ move.angle };
    if (angle > 0.f && angle <= ang1)
    {
        dir = directionType::downRight;
    }
    else if (angle > ang1 && angle <= PI - ang1)
    {
        dir = directionType::down;
    }
    else if (angle > PI - ang1 && angle <= PI)
    {
        dir = directionType::downLeft;
    }
    else if (angle < -PI + ang1 && angle >= -PI)
    {
        dir = directionType::upLeft;
    }
    else if (angle < -ang1 && angle >= -PI + ang1)
    {
        dir = directionType::up;
    }
    else if (angle < 0 && angle >= -ang1)
    {
        dir = directionType::upRight;
    }
}