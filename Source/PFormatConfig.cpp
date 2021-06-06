/*
  ==============================================================================

    PFormatConfig.cpp
    Created: 5 Jun 2021 12:44:01am
    Author:  Haydon Cardew

  ==============================================================================
*/

#include "PFormatConfig.h"

PFormatConfigs::PFormatConfigs()
{
    constructConfigs();
}

void PFormatConfigs::constructConfigs()
{
    configs.push_back(
    make_shared<PFormatConfig>("Tetrahedron",
                               vector<vector<float>>
    {
       {1,  1,  1,  1},
       {1,  1, -1, -1},
       {1, -1,  1, -1},
       {1, -1, -1,  1}
   }));
}

pFormatCoefs PFormatConfigs::getCoefficients(string name, unsigned ambiOrder)
{
    auto config = getConfig(name);
    return config->getCoeffcients(ambiOrder);
}

shared_ptr<PFormatConfig> PFormatConfigs::getConfig(string configName)
{
    for (int i = 0; i < configs.size(); ++i)
    {
        if (configs[i]->getName() == configName)
        {
            return configs[i];
        }
    }
    // this needs to be more bullet proof
    assert(false);
    return configs[0];
}

vector<string> PFormatConfigs::getConfigsNames()
{
    vector<string> names(configs.size());
    for (int i = 0; i < configs.size(); ++i)
    {
        names[i] = configs[i]->getName();
    }
    return names;
}

int PFormatConfigs::getMaxPFormatChannels()
{
    int maxChannels = 0;
    for (auto config : configs)
    {
        if (config->getNumPFormatChannels() > maxChannels)
        {
            maxChannels = config->getNumPFormatChannels();
        }
    }
    return maxChannels;
}

PFormatConfig::PFormatConfig(string name, vector<vector<float>> coefficients) : name(name), coefficients(coefficients)
{
};

string PFormatConfig::getName() const
{
    return name;
}

int PFormatConfig::getNumPFormatChannels() const
{
    return static_cast<int>(coefficients.size());
}

pFormatCoefs PFormatConfig::getCoeffcients(unsigned ambiOrder) const
{
    const int nChannels = pow(ambiOrder+1, 2);
    vector<vector<float>> coefs(coefficients);
    for (auto & c : coefs)
    {
        assert(c.size() >= nChannels); // this shoul dbe checked elsewhere in the plugin
        c.resize(nChannels);
    }
    return coefs;
}
