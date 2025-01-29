#ifndef ENUMS_H
#define ENUMS_H

#include <SFML/Graphics.hpp>
#include <array>
#include <chrono>
#include <unordered_set>
#include <math.h>
#include <string>
#include <utility>

constexpr bool DEBUG_LAUNCH { true };

#define ROWS 12
#define COLUMNS 20

#define INIT_SCALE 2

#define TURN_TIME 90

#define PI 3.14159265

const int WIDTH2{ 1920 };
const int HEIGHT2{ 1080 };

const double SQ_WIDTH{ 48 };
const double SQ_HEIGHT{ 36 };

const double TILE_WIDTH{ 48 };
const double TILE_HEIGHT{ 27 };

const double EFFECT_WIDTH{ 48 };
const double EFFECT_HEIGHT{ 48 };

constexpr double FIRE_SPREAD_TIME { 1.0 };

typedef std::pair<int, int> TileIdx;
typedef std::pair<float, float> PosInTile;
typedef std::pair<float, float> VelInTile;

typedef std::pair<float, float> VelInTile;

typedef struct screenPos {
    float x;
    float y;
} screenPos;

typedef struct worldPos {
    double x;
    double y;
    double z;
} worldPos;

typedef struct textureSize {
    uint16_t w;
    uint16_t h;
} textureSize;

typedef std::chrono::system_clock Time;
typedef std::chrono::time_point<Time> timePoint;
typedef std::chrono::duration<double> timeDuration;

typedef struct playerStats {
    uint16_t score;
    uint16_t kills;
    uint16_t deaths;
} playerStats;

typedef struct GameDeltas {
    std::unordered_set<int> tilesWithDelta;
    std::unordered_set<int> wosWithDelta;
    std::unordered_set<int> wosToDelete;
} GameDeltas;

const TileIdx OOB_TILE{ 300, 300 }; // Out of Bounds

enum class tileType 
{   
    grass,
    mountain,
    water,
    bridge,
    burnedGround,
    volcano,
    ice,
    snow,
    last
};
const int N_BLOCKTYPES{ 6 };

enum class SpellType
{
    fireball,
    teleport,
    life,
    wind,
    tornado,
    freeze,
    swap,
    portal,
    last
};

enum class EffectType
{   
    fire,
    wind,
    eruption,
    sink,
    slide,
    stop,
    spawn,
    spellBook,
    portal,
    none,
    last
};

enum class WorldObjectType
{
    player,
    tree,
    grass,
    mountain,
    volcano,
    spell,
    fireball,
    visual,
    remove,
    last
};

enum class VisualType
{
    aim,
    aimDir,
    dot,
    last
};

enum class directionType 
{   up,
    upLeft,
    downLeft,
    down,
    downRight,
    upRight,
    none
};

enum class burnType 
{   beginBurning,
    burning,
    endBurning,
    burnt,
    normal,
    none
};

typedef struct collisionOutcome {
    WorldObjectType movedWO;
    WorldObjectType worldObject;
} collisionOutcome;

typedef struct spellOutcome {
    tileType tile;
    EffectType effect;
} spellOutcome;

typedef struct castedSpellData {
    int id;
    int playerId;
    SpellType spellType;
    directionType dir;
    std::chrono::time_point<std::chrono::system_clock> startTime;
    double traveledPerc;
    TileIdx fromTileIdx;
    TileIdx toTileIdx;
} castedSpellData;

typedef struct effectData {
    EffectType type;
    directionType direction;
    bool active;
} effectData;

inline double GetSpeed(SpellType spell)
{
    switch (spell)
    {
        case SpellType::fireball:
        case SpellType::life:
        case SpellType::freeze:
        case SpellType::swap:
            return 1.0;
        case SpellType::teleport:
            return 2.5;
        default:
            return -1.0;
    }
}

inline collisionOutcome GetWorldObjectsFromCollision(WorldObjectType movedWO, WorldObjectType woType)
{
    switch (movedWO)
    {
    case WorldObjectType::fireball:
        switch (woType)
        {
        case WorldObjectType::mountain:
            return {WorldObjectType::remove, WorldObjectType::volcano};
        default:
            return {WorldObjectType::remove, WorldObjectType::last};
        }
    default:
        return {WorldObjectType::last, WorldObjectType::last};
    }
}

inline tileType GetTileFromCollision(tileType tile, WorldObjectType woType)
{
    switch (woType)
    {
    case WorldObjectType::fireball:
        switch (tile)
        {
        case tileType::grass:
            return {tileType::burnedGround};

        case tileType::mountain:
            return {tileType::volcano};

        case tileType::burnedGround:
            return {tileType::burnedGround};

        case tileType::snow:
            return {tileType::grass};

        default:
            return {tileType::last};
        }
    default:
        return {tileType::last};
    }
}

// TODO: Is this in use now or shoukd it be removed?
inline spellOutcome GetTileFromCastSpell(tileType tile, SpellType spell)
{
    switch (spell)
    {
    case SpellType::fireball:
        switch (tile)
        {
        case tileType::grass:
            return {tileType::burnedGround, EffectType::fire};

        case tileType::mountain:
            return {tileType::volcano, EffectType::eruption};

        case tileType::burnedGround:
            return {tileType::burnedGround, EffectType::fire};

        case tileType::snow:
            return {tileType::grass, EffectType::fire};

        default:
            return {tileType::last, EffectType::last};
        }
    case SpellType::teleport:
        return {tileType::last, EffectType::last};
    case SpellType::life:
        return {tileType::last, EffectType::last};
    case SpellType::wind:
        switch (tile)
        {
        case tileType::grass:
        case tileType::burnedGround:
        case tileType::snow:
        case tileType::ice:
            return {tileType::last, EffectType::wind};
        default:
            return {tileType::last, EffectType::last};
        }
        return {tileType::last, EffectType::last};
    case SpellType::tornado:
        return {tileType::last, EffectType::last};
    case SpellType::freeze:
        switch (tile)
        {
        case tileType::grass:
            return {tileType::snow, EffectType::last};

        case tileType::mountain:
            return {tileType::volcano, EffectType::eruption};

        case tileType::water:
            return {tileType::ice, EffectType::slide};

        case tileType::volcano:
            return {tileType::mountain, EffectType::last}; 

        default:
            return {tileType::last, EffectType::last};
        }
    case SpellType::swap:
        return {tileType::last, EffectType::last};

    default:
        return {tileType::last, EffectType::last};
    }
}

inline EffectType GetInheritEffect(tileType tile)
{
    switch (tile)
    {
        case tileType::grass:        return EffectType::none;
        case tileType::bridge:       return EffectType::none;
        case tileType::mountain:     return EffectType::stop;
        case tileType::water:        return EffectType::sink;
        case tileType::burnedGround: return EffectType::none;
        case tileType::volcano:      return EffectType::stop;
        case tileType::ice:          return EffectType::slide;
        case tileType::snow:         return EffectType::none;
        default:                     return EffectType::last;
    }
}

inline bool HasTexture(EffectType effect)
{
    switch (effect)
    {
        case EffectType::fire:
        case EffectType::wind:
        case EffectType::spawn:
        case EffectType::portal:
            return true;
        default:
            return false;
    }
}

inline textureSize GetSize(WorldObjectType objectType, int id)
{
    switch (objectType)
    {
        case WorldObjectType::player:
            return {16, 32};
        case WorldObjectType::tree:
            return {16, 60};
        case WorldObjectType::grass:
            return {10, 5};
        case WorldObjectType::mountain:
            return {48, 44};
        case WorldObjectType::volcano:
            return {48, 44};
        case WorldObjectType::fireball:
            return {10, 10};
        case WorldObjectType::spell:
            return {16, 16};
        case WorldObjectType::visual:
            switch (static_cast<VisualType>(id))
            {
                case VisualType::aim:
                    return {5, 5};
                case VisualType::aimDir:
                    return {16, 16};
                case VisualType::dot:
                    return {1, 1};
            }
        default:
            return {1, 1};
    }
}

inline sf::Vector2f GetHitbox(WorldObjectType objectType)
{
    switch (objectType)
    {
        case WorldObjectType::player:
            return {14.f, 28.f};
        case WorldObjectType::mountain:
            return {44.f, 44.f - 12.5f};
        case WorldObjectType::volcano:
            return {44.f, 44.f - 12.5f};
        case WorldObjectType::fireball:
            return {10.f, 10.f};
        default:
            return {1.f, 1.f};
    }
}

inline sf::Vector2f GetTextureOrigin(WorldObjectType objectType, sf::Vector2f dim)
{
    switch (objectType)
    {
        case WorldObjectType::player:
            return {dim.x / 2.f, dim.y / 2.f + 12.f};
        case WorldObjectType::grass:
            return {dim.x / 2.f, dim.y / 2.f + 2.f};
        case WorldObjectType::mountain:
            return {dim.x / 2.f, dim.y - 12.5f};
        case WorldObjectType::volcano:
            return {dim.x / 2.f, dim.y - 12.5f};
        default:
            return {dim.x / 2.f, dim.y / 2.f};
    }
}

inline bool GetCanFall(WorldObjectType objectType)
{
    switch (objectType)
    {
        case WorldObjectType::player:
            return true;
        default:
            return false;
    }
}

inline bool GetCollideable(WorldObjectType objectType)
{
    switch (objectType)
    {
        case WorldObjectType::visual:
            return false;
        default:
            return true;
    }
}

inline bool IsDirectional(SpellType spell)
{
    switch (spell)
    {
        case SpellType::wind:
            return true;
        default:
            return false;
    }
}

inline std::string ToString(tileType tile)
{
    switch (tile)
    {
        case tileType::grass:          return "Grass";
        case tileType::bridge:         return "Bridge";
        case tileType::mountain:       return "Mountain";
        case tileType::water:          return "Water";
        case tileType::ice:            return "Ice";
        case tileType::volcano:        return "Volcano";
        case tileType::burnedGround:   return "BurnedGround";
        case tileType::snow:           return "Snow";
        default:      return "[Unknown Tile]";
    }
}

inline std::string ToString(WorldObjectType worldObject)
{
    switch (worldObject)
    {
        case WorldObjectType::player:            return "Player";
        case WorldObjectType::tree:              return "Tree";
        case WorldObjectType::grass:             return "Grass";
        case WorldObjectType::mountain:          return "Mountain";
        case WorldObjectType::volcano:           return "Volcano";
        case WorldObjectType::spell:             return "Spell";
        case WorldObjectType::fireball:          return "Fireball";
        case WorldObjectType::visual:            return "Visual";
        case WorldObjectType::remove:            return "Remove";
        default:      return "[Unknown WorldObjectType]";
    }
}

//std::array<int, 3> OBJECT_PROB{ 60, 30, 10 }; // grass, mountain, water
//std::array<int, 2> CONNECTOR_PROB{ 90, 10 }; // is, And
//const int N_TEXTTYPES{ 13 };

inline std::string ToString(int id)
{
    switch (id)
    {
        case 0:   return "A";
        case 1:   return "B";
        case 2:   return "C";
        case 3:   return "D";
        case 4:   return "E";
        default:  return "[Unknown Player Id]";
    }
}

inline std::string ToString(SpellType spell)
{
    switch (spell)
    {
        case SpellType::fireball:   return "Fireball";
        case SpellType::teleport:   return "Teleport";
        case SpellType::life:       return "Life";
        case SpellType::wind:       return "Wind";
        case SpellType::freeze:     return "Freeze";
        case SpellType::swap:       return "Swap";
        case SpellType::portal:     return "Portal";
        default:      return "[Unknown Spell]";
    }
}

inline std::string ToString(EffectType effect)
{
    switch (effect)
    {
        case EffectType::fire:        return "Fire";
        case EffectType::wind:        return "Wind";
        case EffectType::spawn:       return "Spawn";
        case EffectType::spellBook:   return "SpellBook";
        default:      return "[Unknown Effect]";
    }
}

inline std::string ToString(VisualType visual)
{
    switch (visual)
    {
        case VisualType::aim:         return "Aim";
        case VisualType::aimDir:      return "Arrow";
        case VisualType::dot:         return "Dot";
        default:      return "[Unknown Visual]";
    }
}

inline std::string ToString(directionType direction)
{
    switch (direction)
    {
        case directionType::up:          return "U";
        case directionType::upLeft:      return "UL";
        case directionType::downLeft:    return "DL";
        case directionType::down:        return "D";
        case directionType::downRight:   return "DR";
        case directionType::upRight:     return "UR";
        default:      return "[Unknown Direction]";
    }
}

inline std::string GetFolderAndFile(WorldObjectType worldObject, int id)
{
    std::string name;
    switch (worldObject)
    {
        case WorldObjectType::player:
            return "Players/Player" + ToString(id) + "/Player" + ToString(id);
        case WorldObjectType::grass:
            return "Grass/Grass";
        case WorldObjectType::mountain:
            return "Mountain/Mountain";
        case WorldObjectType::volcano:
            return "Volcano/Volcano";
        case WorldObjectType::fireball:
            return "Fireball/Fireball";
        case WorldObjectType::spell:
            name = ToString(static_cast<SpellType>(id));
            return "Spells/" + name + "/" + name;
        case WorldObjectType::visual:
            name = ToString(static_cast<VisualType>(id));
            return "Visuals/" + name + "/" + name;
        default:      return "[Unknown WorldObjectType for GetFolderAndFile]";
    }
}

enum class textureType 
{   directional,
    normal,
    none,
    last
};

inline textureType GetTextureType(tileType tile)
{
    switch (tile)
    {
        default:
            return textureType::normal;
    }
}

inline textureType GetTextureType(WorldObjectType objectType)
{
    switch (objectType)
    {
        case WorldObjectType::player:
            return textureType::directional;
        default:
            return textureType::normal;
    }
}

inline directionType GetInitDir(WorldObjectType objectType)
{
    textureType textureT{ GetTextureType(objectType) };
    switch (textureT)
    {
        case textureType::directional:
            return directionType::up;
        default:
            return directionType::none;
    }
}

inline textureType GetTextureType(EffectType effect)
{
    switch (effect)
    {
        case EffectType::wind:
            return textureType::directional;
        case EffectType::fire:
        case EffectType::spawn:
        case EffectType::portal:
            return textureType::normal;
        default:
            return textureType::none;
    }
}

inline textureType GetTextureType(VisualType visual)
{
    switch (visual)
    {
        case VisualType::aimDir:
            return textureType::directional;
        default:
            return textureType::normal;
    }
}

enum class actionType
{   up,
    upLeft,
    downLeft,
    down,
    downRight,
    upRight,
    undoMove,
    skipTurn,
    none
};

inline directionType GetDirectionType(actionType action)
{
    switch (action)
    {
    case actionType::up:
        return directionType::up;
    case actionType::upLeft:
        return directionType::upLeft;
    case actionType::downLeft:
        return directionType::downLeft;
    case actionType::down:
        return directionType::down;
    case actionType::downRight:
        return directionType::downRight;
    case actionType::upRight:
        return directionType::upRight;
    default:
        return directionType::none;
    }
}

inline directionType RotateDirection(directionType dir)
{
    switch (dir)
    {
    case directionType::up:
        return directionType::upLeft;
    case directionType::upLeft:
        return directionType::downLeft;
    case directionType::downLeft:
        return directionType::down;
    case directionType::down:
        return directionType::downRight;
    case directionType::downRight:
        return directionType::upRight;
    case directionType::upRight:
        return directionType::up;
    default:
        return directionType::none;
    }
}

enum class stateType 
{   playerTurn, 
    lobby, 
    generateLevel
};

// ################# Input Stuff ######################
enum class xbox
{
    A,
    B,
    X,
    Y,
    LB,
    RB,
    window,
    burger,
    LJoy,
    RJoy,
    Xbox,
    lift,
    none,
    last
};

enum class LJoyMode
{
    move,
    aim,
    last
};

typedef struct moveInput {
    float angle;
    float power;
} moveInput;

typedef struct gameInput {
    xbox button;
    actionType action;
    moveInput move;
} gameInput;

typedef struct displayInput {
    float zoom;
    float horizontal; 
    float vertical;
    bool tileSpacing;
    bool showCoordinateSystem;
    bool enableShaders;
} displayInput;

constexpr displayInput CAMERA_0{ 1.0 , WIDTH2 / 2, HEIGHT2 / 2, false, false, false };

#endif