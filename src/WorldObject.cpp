#include <unordered_set>
#include <cmath>
#include "WorldObject.h"
#include "Transformations.h"

// Todo  38.f was derived through testing just. Magick number
WorldObject::WorldObject(uint16_t id, WorldObjectType type,  worldPos w_pos, std::map<uint8_t, GameDeltas>* deltas)
    : id{ id }, type{ type }, pos{ w_pos }, vel{ 0, 0, 0 }, acc{ 0, 0, 0 }, tileIdx{ WorldPosToTileIdx(w_pos) }, dir{ GetInitDir(type) },
      width{ GetHitbox(type).x / 42.f },
      height{ GetHitbox(type).y / 42.f },
      origin{}, moving{ false }, takeInput{true}, tileLeavable{true}, fallable{ GetCanFall(type) }, collideable{ GetCollideable(type) }, effects{}, deltas{ deltas }
{
    for (auto& [playerId, delta] : *deltas)
        delta.wosWithDelta.insert(id);
}

WorldObject::WorldObject()
    : id{}, type{}, pos{}, vel{}, acc{}, tileIdx{}, dir{}, width{}, height{}, origin{}, moving{}, takeInput{}, tileLeavable{}, fallable{}, collideable{}, effects{}, deltas{}
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

void WorldObject::setType(WorldObjectType newType)
{
    type = newType;
    for (auto& [playerId, delta] : *deltas)
        delta.wosWithDelta.insert(id);
}

worldPos WorldObject::getPos()
{
    return pos;
}

void WorldObject::setPos(worldPos w_pos)
{
    pos = w_pos;
    tileIdx = WorldPosToTileIdx(w_pos);
    // Change so that this doesn't happen if pos is unchanged.
    for (auto& [playerId, delta] : *deltas)
        delta.wosWithDelta.insert(id);
}

directionType WorldObject::getDir()
{
    return dir;
}

float WorldObject::getWidth()
{
    return width;
}

float WorldObject::getHeight()
{
    return height;
}

TileIdx WorldObject::getTileIdx()
{
    return tileIdx;
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

void WorldObject::setCanLeaveTile(bool canLeave)
{
    tileLeavable = canLeave;
}

bool WorldObject::canLeaveTile()
{
    return tileLeavable;
}

bool WorldObject::isIntersecting(WorldObject& worldObject)
{
    // Assume Cylinder shape, without rotations
    bool intersecting{ false };
    worldPos inPos{ WorldPosToCartesian(worldObject.getPos()) };
    worldPos selfPos{ WorldPosToCartesian(pos) };
    float d{ float(pow(inPos.x - selfPos.x, 2) + pow(inPos.y - selfPos.y, 2)) };
    float r_sum{ float(pow((worldObject.getWidth() + width) / 2.f, 2)) };

    if (d < r_sum)
    {
        if (inPos.z < pos.z)
        {
            if ((inPos.z + worldObject.getHeight()) > pos.z)
                intersecting = true;
        }
        else
        {
            if (inPos.z < (pos.z + height))
                intersecting = true;
        }

        if (intersecting && collideable && worldObject.isCollideable())
        {
            float len{ std::sqrt(d) };
            float okLen{ std::sqrt(r_sum) };
            worldPos newPos{
                CartesianToWorldPos(worldPos{ 
                    inPos.x + (selfPos.x - inPos.x) * (okLen / len),
                    inPos.y + (selfPos.y - inPos.y) * (okLen / len),
                    0
            })};
            pos.x = newPos.x;
            pos.y = newPos.y;
        }
    }
    return intersecting;
}

void WorldObject::canTakeInput(bool can)
{
    takeInput = can;
}

bool WorldObject::canFall()
{
    return fallable;
}

bool WorldObject::isCollideable()
{
    return collideable;
}

void WorldObject::setVelocity(worldPos& velIn)
{
    vel = velIn;
    for (auto& [playerId, delta] : *deltas)
        delta.wosWithDelta.insert(id);
    // Temp: Got linking problems when trying to make this a function somewhere
    double epsilon = 0.0001;
    moving = !(std::fabs(vel.x) < epsilon && std::fabs(vel.y) < epsilon && std::fabs(vel.z) < epsilon);
}

// Not completely sure if this should be here instead of in Player
void WorldObject::setVelocity(moveInput& move)
{
    if (takeInput)
        vel = InputToWorldVel(move);
    for (auto& [playerId, delta] : *deltas)
        delta.wosWithDelta.insert(id);

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
    m.origin = origin;

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
    origin = m.origin;

    tileIdx = WorldPosToTileIdx(m.pos);
}