#ifndef FIRE_H
#define FIRE_H

#include <vector>
#include <chrono>
#include "Enums.h"


//template <std::size_t W, std::size_t H>
class Fire
{
private:
    uint16_t id;
    uint16_t pixelWidth;
    uint16_t pixelHeight;
    std::vector<std::vector<burnType>> matrix;
    timePoint lastUpdate;

public:
    Fire(uint16_t id, uint16_t pixelWidth, uint16_t pixelHeight);
    ~Fire();

    void igniteNeighbours(int i, int j);
    void update();
    void reset();
};

#endif