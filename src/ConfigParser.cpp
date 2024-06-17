#include <fstream> 
#include <iostream> 
#include <sstream>
#include <string> 
#include <vector>
#include "ConfigParser.h"

ConfigParser::ConfigParser(std::string configPath)
    : configPath{ configPath }
{}

ConfigParser::~ConfigParser()
{
}

void ConfigParser::loadConfig(double& joyThreshHigh, double& joyThreshLow, unsigned& seed)
{
    std::ifstream configFile{ configPath }; 
    std::string line; 
  
    while (getline(configFile, line))
    { 
        std::stringstream ss{ line };
        std::string word;
        std::vector<std::string> lineV;
        while (ss >> word)
        {
            if (word == "JOYSTICK_THRESHOLD_HIGH")
            {
                ss >> word;  // To get rid of =
                ss >> joyThreshHigh;
            }
            else if (word == "JOYSTICK_THRESHOLD_LOW")
            {
                ss >> word;  // To get rid of =
                ss >> joyThreshLow;
            }
            else if (word == "SEED")
            {
                ss >> word;  // To get rid of =
                ss >> seed;
            }
        }
    }

    configFile.close();
    std::cout << "Loaded Config" << std::endl;
}