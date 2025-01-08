#include <unordered_set>
#include "WorldObject.h"
#include "Transformations.h"

WorldObject::WorldObject(uint16_t id, WorldObjectType type,  worldPos w_pos, std::unordered_set<int>* wosWithDelta)
    : id{ id }, type{ type }, pos{ w_pos }, vel{ 0, 0, 0 }, acc{ 0, 0, 0 }, dir{ GetInitDir(type) }, width{}, height{}, moving{ false }, effects{}, wosWithDelta{ wosWithDelta }
{
    wosWithDelta->insert(id);
}

WorldObject::WorldObject()
    : id{}, type{}, pos{}, vel{}, acc{}, dir{}, width{}, height{}, moving{}, effects{}, wosWithDelta{}
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
    // Change so that this doesn't happen if pos is unchanged.
    wosWithDelta->insert(id);
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

bool WorldObject::isMoving()
{
    return moving;
}

// Not completely sure if this should be here instead of in Player
void WorldObject::setVelocity(moveInput& move)
{
    vel = InputToWorldVel(move);
    wosWithDelta->insert(id);

    // Temp: Got linking problems when trying to make this a function somewhere
    double epsilon = 0.0001;
    moving = !(std::fabs(vel.x) < epsilon && std::fabs(vel.y) < epsilon && std::fabs(vel.z) < epsilon);

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

void WorldObject::getAllData(WorldObjectStruct& m) const
{
    m.id     = id;
    m.type   = type;
    m.pos    = pos;
    m.vel    = vel;
    m.acc    = acc;
    m.dir    = dir;
    m.width  = width;
    m.height = height;

}

void WorldObject::setAllData(WorldObjectStruct& m)
{
    id     = m.id;
    type   = m.type;
    pos    = m.pos;
    vel    = m.vel;
    acc    = m.acc;
    dir    = m.dir;
    width  = m.width;
    height = m.height;
}