#include "Fire.h"

Fire::Fire(uint16_t idT, uint16_t pixelWidthT, uint16_t pixelHeightT)
    : id{ idT }, pixelWidth{ pixelWidthT }, pixelHeight{ pixelHeightT }
    , matrix{ pixelHeightT, std::vector<burnType>(pixelWidthT, burnType::normal) }
    , lastUpdate{ Time::now() }
{}

Fire::~Fire()
{
}

void Fire::igniteNeighbours(int i, int j)
{
    for(int n{-1}; n < 2; n++){
        int ii{ i + n };
        if(ii < 0 || ii >= matrix.size())
            break;
        for(int m{-1}; m < 2; m++){
            int jj{ j + m };
            if(jj < 0 || jj >= matrix.at(0).size())
                break;

            auto& element{ matrix.at(ii).at(jj) };
            if (element == burnType::normal)
                element = burnType::beginBurning;
        }
    }
}

void Fire::update()
{
    timePoint currentTime{ Time::now() };
    timeDuration timePast{ currentTime - lastUpdate };

    if (timePast.count() > FIRE_SPREAD_TIME)
    {
        lastUpdate = currentTime;

        for(int i{0}; i < matrix.size(); i++){
            auto& row = matrix.at(i);
            for(int j{0}; j < row.size(); j++){
                auto& element = row.at(j);
                switch (element)
                {
                case burnType::burning:
                    igniteNeighbours(i, j);
                    element = burnType::endBurning;
                    break;

                case burnType::endBurning:
                    element = burnType::burnt;
                    break;

                default:
                    break;
                }
            }
        }

        for(auto& row : matrix){
            for(auto& element : row){
                if (element == burnType::beginBurning)
                    element = burnType::burning;
            }
        }
    }
}