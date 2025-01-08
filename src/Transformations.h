#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include "Enums.h"

inline worldPos InputToWorldVel(moveInput moveInput)
{
    worldPos w_vel;
    double screen_x = moveInput.power * std::cos(-moveInput.angle);
    double neg_screen_y = moveInput.power * std::sin(-moveInput.angle);

    w_vel.x = screen_x - neg_screen_y;
    w_vel.y = screen_x + neg_screen_y;
    w_vel.z = 0;

    return w_vel;
}

inline worldPos TileIdxToWorldPos(TileIdx tileIdx)
{
    worldPos w_pos;

    w_pos.x = static_cast<double>(tileIdx.first) + 
              static_cast<double>(tileIdx.second);
    w_pos.y = - static_cast<double>(tileIdx.first) * 0.5
              + static_cast<double>(tileIdx.second) * 0.5;

    w_pos.z = 0;

    return w_pos;
}

// Inverse of TileIdxToWorldPos
inline TileIdx WorldPosToTileIdx(worldPos w_pos)
{
    TileIdx tileIdx;

    double r{ w_pos.x * 0.5 - w_pos.y };
    double c{ w_pos.x * 0.5 + w_pos.y };
     
    tileIdx = {static_cast<int>(round(r)),
               static_cast<int>(round(c))};

    return tileIdx;
}

inline screenPos WorldToScreenPos(worldPos worldPos, const displayInput& camera)
{
    screenPos s_pos;

    s_pos.x = static_cast<float>(
                camera.horizontal +
                (worldPos.x * 24.0  + worldPos.y * 32.0) * camera.zoom);
    s_pos.y = static_cast<float>(
                camera.vertical +
                (worldPos.x * 12.0  - worldPos.y * 16.0 - worldPos.z * 16.0) * camera.zoom); 
    
    return s_pos;
}

inline worldPos WorldToNormalVec(worldPos worldVec)
{    
    worldPos n_vec;

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