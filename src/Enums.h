#ifndef ENUMS_H
#define ENUMS_H

#include <SFML/Graphics.hpp>
#include <array>
#include <chrono>
#include <string>
#include <utility>

#define ROWS 12
#define COLUMNS 20

#define INIT_SCALE 2

const double SQ_WIDTH{ 48 };
const double SQ_HEIGHT{ 36 };

const double WIZARD_WIDTH{ 16 };
const double WIZARD_HEIGHT{ 32 };

const double SPELL_WIDTH{ 16 };
const double SPELL_HEIGHT{ 16 };

const double EFFECT_WIDTH{ 48 };
const double EFFECT_HEIGHT{ 48 };

const double VISUAL_WIDTH{ 16 };
const double VISUAL_HEIGHT{ 16 };

#define WIDTH2 1920
#define HEIGHT2 1080

#define TURN_TIME 90

#define PI 3.14159265

typedef std::pair<int, int> TileIdx;

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

enum class VisualType
{
    aimDir,
    last
};

enum class EntityType
{
    player,
    spell,
    effect,
    mob,
    item,
    visual,
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

//std::array<int, 3> OBJECT_PROB{ 60, 30, 10 }; // grass, mountain, water
//std::array<int, 2> CONNECTOR_PROB{ 90, 10 }; // is, And
//const int N_TEXTTYPES{ 13 };

inline std::string ToString(int id)
{
    switch (id)
    {
        case 0:   return "A";
        case 1:   return "B";
        case 3:   return "C";
        case 4:   return "D";
        case 5:   return "E";
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
        case VisualType::aimDir:      return "Arrow";
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

enum class gameEventType
{   generateLevel,
    none
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

typedef struct gameInput {
    xbox button;
    actionType action;
} gameInput;

typedef struct displayInput {
    float zoom;
    float horizontal; 
    float vertical;
    bool tileSpacing;
} displayInput;

#endif