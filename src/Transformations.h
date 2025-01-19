#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include "Enums.h"

inline worldPos WorldPosToCartesian(worldPos w_pos)
{
    worldPos cartes;

    double a1{ 0.86603 }; // cos(30)
    double a2{ 0.5 };     // sin(30)

    cartes.x =  w_pos.x * a1 + w_pos.y * a1;
    cartes.y = -w_pos.x * a2 + w_pos.y * a2;
    cartes.z =  w_pos.z;

    return cartes;
}

inline worldPos CartesianToWorldPos(worldPos cartes)
{
    worldPos w_pos;

    double a3{ 0.57735 }; // sin(30)/cos(30)
    double a4{ 1.0 };

    w_pos.x = cartes.x * a3 - cartes.y * a4;
    w_pos.y = cartes.x * a3 + cartes.y * a4;
    w_pos.z = cartes.z;

    return w_pos;
}

// TODO: Doesn't handle z yet
inline double worldPosLen(worldPos w_pos)
{
    // c^2 = a^2 + b^2 -2ab*cos(gamma), gamma = 180 - 60 = 120, cos(120) = -0.5, -2 * -0.5 = 1
    double len{ std::sqrt(std::pow(w_pos.x, 2) + std::pow(w_pos.y, 2) + w_pos.x * w_pos.y) };

    return len;
}

inline worldPos InputToWorldVel(moveInput moveInput)
{
    worldPos w_vel;
    double screen_x = moveInput.power * std::cos(-moveInput.angle);
    double neg_screen_y = moveInput.power * std::sin(-moveInput.angle);

    w_vel.x = screen_x - neg_screen_y;
    w_vel.y = screen_x + neg_screen_y;
    w_vel.z = 0;

    w_vel = CartesianToWorldPos(w_vel);

    return w_vel;
}

inline worldPos TileIdxToWorldPos(TileIdx tileIdx)
{
    worldPos w_pos;

    w_pos.x = static_cast<double>(tileIdx.first);
    w_pos.y = static_cast<double>(tileIdx.second);

    w_pos.z = 0;

    return w_pos;
}

// TODO: This can't be the most efficient way
inline TileIdx WorldPosToTileIdx(worldPos w_pos)
{
    std::array<TileIdx, 3> tileIdxs{
        TileIdx{ int(round(w_pos.x)), int(round(w_pos.y)) },
        TileIdx{ int(floor(w_pos.x)), int(round(w_pos.y)) },
        TileIdx{ int(round(w_pos.x)), int(floor(w_pos.y)) }
    };

    int closestId{ 0 };
    double closestLen;
    int id{ 0 };
    for (auto& tileIdx : tileIdxs)
    {
        worldPos localPos{
            w_pos.x - tileIdx.first,
            w_pos.y - tileIdx.second,
            0.0
        };
        double len{ worldPosLen(localPos) };
        if (len < 0.5)
        {
            closestId = id;
            break;
        }
        else if (id == 0)
        {
            closestLen = len;
        }
        else if (len < closestLen)
        {
            closestLen = len;
            closestId = id;
        }
        id++;
    }

    return tileIdxs.at(closestId);
}

inline screenPos WorldToScreenPos(worldPos w_pos, const displayInput& camera)
{
    screenPos s_pos;

    //w_pos = WorldPosToCartesian(w_pos);
    s_pos.x = static_cast<float>(
                camera.horizontal +
                (w_pos.x * 8.0 + w_pos.y * 40.0) * camera.zoom);
                // w_pos.x * 24.0  + w_pos.y * 32.0  
    s_pos.y = static_cast<float>(
                camera.vertical +
                (w_pos.x * 20.0 + w_pos.y * 4.0 - w_pos.z * 16.0) * camera.zoom); 
                // w_pos.x * 12.0  - w_pos.y * 16.0 
    
    return s_pos;
}

inline worldPos WorldToNormalVec(worldPos worldVec)
{    
    worldPos n_vec;

    worldVec = WorldPosToCartesian(worldVec);
    float a1{ 0.89443 }; // sin(arctan(2))
    float a2{ 0.44721 }; // cos(arctan(2))
    float a3{ 0.86603 }; // cos(arcsin(0.5)) = sin(arccos(0.5))

    n_vec.x = static_cast<float>( worldVec.x * a1 + worldVec.y * a1);
    n_vec.y = static_cast<float>(-worldVec.x * a2 + worldVec.y * a2 + worldVec.z * a3);
    n_vec.z = static_cast<float>(-worldVec.x * a3 + worldVec.y * a3 + worldVec.z * 0.5);

    return n_vec;
}

inline worldPos NormalToWorldVec(worldPos n_vec)
{    
    worldPos worldVec;

    // Calculated inverse matrix using Python
    float b1{ 0.55902 }; 
    float b2{ 0.47492 }; 
    float b3{ 0.82259 };
    float b4{ 1.64518 }; 
    float b5{ 0.84956 }; 

    worldVec.x = static_cast<float>(n_vec.x * b1 + n_vec.y * b2 - n_vec.z * b3);
    worldVec.y = static_cast<float>(n_vec.x * b1 - n_vec.y * b2 + n_vec.z * b3);
    worldVec.z = static_cast<float>(               n_vec.y * b4 - n_vec.z * b5);

    worldVec = CartesianToWorldPos(worldVec);
    return worldVec;
}

inline worldPos NormalToScreenVec(worldPos n_vec)
{    
    worldPos s_vec;

    s_vec.x =  n_vec.x;
    s_vec.y = -n_vec.y;
    s_vec.z = -n_vec.z;

    return s_vec;
}

/*
bool IsZero(worldPos vec, double epsilon = 0.0001)
{
    return std::fabs(vec.x) < epsilon && std::fabs(vec.y) < epsilon && std::fabs(vec.z) < epsilon;
}*/

#endif