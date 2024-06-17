#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

class ConfigParser
{
private:
    std::string configPath;

public:
    ConfigParser(std::string configPath);
    ~ConfigParser();

    void loadConfig(double& joyThreshHigh, double& joyThreshLow, unsigned& seed);
};

#endif