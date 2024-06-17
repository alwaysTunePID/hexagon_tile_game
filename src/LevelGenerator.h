#ifndef LEVELGENERATOR_H
#define LEVELGENERATOR_H

#include <vector>
#include <map>
#include <random>
#include "Tile.h"

class LevelGenerator
{
private:

    unsigned seed;
    std::default_random_engine generator;
    std::map<int, Tile> blocks;

    std::vector<EffectType> all_objects;
    std::vector<EffectType> all_connectors;
    std::vector<EffectType> all_properties;

    std::vector<EffectType> objects;
    std::vector<EffectType> connectors;
    std::vector<EffectType> properties;

    bool validLevelInit;

public:
    LevelGenerator(/* args */);
    ~LevelGenerator();

    void generateNewLevel();
    bool createWords(
        std::vector<std::vector<EffectType>>& words);
    bool buildOnWord(
        std::vector<EffectType>& word,
        bool& isAdded,
        textCategory nextTextCategory,
        int& i_obj,
        int& i_con,
        int& i_pro);
    std::map<int, Tile>& getTiles();
};

#endif