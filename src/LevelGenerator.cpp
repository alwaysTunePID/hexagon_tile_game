#include <iostream>
#include <vector>
#include <random>
#include "Tile.h"
#include "Enums.h"
#include "LevelGenerator.h"

LevelGenerator::LevelGenerator(/* args */)
    : seed(2952395), generator(seed), blocks{}, all_objects{},
    all_connectors{}, all_properties{}, objects{}, connectors{}, properties{},
    validLevelInit{ false }
{
    for (int i = 0; i < N_TEXTTYPES; i++)
    {
        EffectType effect{ static_cast<EffectType>(i) };
        switch (GetTextCategory(effect))
        {
        case textCategory::object:
            all_objects.push_back(effect);
            break;
        case textCategory::connector:
            all_connectors.push_back(effect);
            break;
        case textCategory::effect:
            all_properties.push_back(effect);
            break;
        default:
            break;
        }
    }
}

LevelGenerator::~LevelGenerator()
{}

void LevelGenerator::generateNewLevel()
{
    objects.clear();
    connectors.clear();
    properties.clear();

    // textCategory: object, connector, effect

    // choose properties

    double mean{ 16.0 };
    double stddev{ 3.0 };
    int min_n{ 6 };

    std::normal_distribution<double> distribution(mean, stddev);

    int n_effect{ 0 };
    while (n_effect < min_n)
    {
        n_effect = int(distribution(generator));
    }

    std::uniform_int_distribution<int> uniform_properties(0, all_properties.size()-1);

    properties.push_back(EffectType::you);
    properties.push_back(EffectType::you2);
    properties.push_back(EffectType::win);

    while (properties.size() < n_effect)
    {
        EffectType effect{ all_properties.at(uniform_properties(generator)) };

        if (effect == EffectType::you || effect == EffectType::you2 || effect == EffectType::win)
            continue;

        properties.push_back(effect);
    }

    // choose connectors

    mean   = n_effect - 2.0;
    stddev = 0.5;
    min_n  = 3;

    std::normal_distribution<double> normal_dist_connectors(mean, stddev);

    int n_connector{ 0 };
    while (n_connector < min_n)
    {
        n_connector = int(normal_dist_connectors(generator));
    }

    std::discrete_distribution<int> dist_connectors{ CONNECTOR_PROB.begin(), CONNECTOR_PROB.end() };

    while (connectors.size() < n_connector)
    {
        EffectType effect{ all_connectors.at(dist_connectors(generator)) };
        connectors.push_back(effect);
    }

    // choose effect objects

    std::normal_distribution<double> normal_dist_objects(mean, stddev);

    int n_object{ 0 };
    while (n_object < min_n)
    {
        n_object = int(normal_dist_objects(generator));
    }

    std::discrete_distribution<int> dist_objects{ OBJECT_PROB.begin(), OBJECT_PROB.end() };

    while (objects.size() < n_object)
    {
        EffectType effect{ all_objects.at(dist_objects(generator)) };
        
        if (std::find(objects.begin(), objects.end(), effect) != objects.end())
        {
            if (effect == EffectType::baba || effect == EffectType::keke || effect == EffectType::flag)
                continue;
        }
        objects.push_back(effect);
    }


    // build words

    std::vector<std::vector<EffectType>> words{};
    createWords(words);

    // choose objects
    tileType::baba

    // create blocks

    // place blocks

}



std::map<int, Tile>& LevelGenerator::getTiles()
{
    return blocks;
}