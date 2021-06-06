/*
  ==============================================================================

    PFormatConfig.h
    Created: 5 Jun 2021 12:44:01am
    Author:  Haydon Cardew

  ==============================================================================
*/

#pragma once

#include <vector>
#include <string>

using namespace std;

typedef vector<vector<float>> pFormatCoefs;

struct PFormatConfig
{
public:
    PFormatConfig(string name, vector<vector<float>> coefficients);
    string getName() const;
    int getNumPFormatChannels() const;
    pFormatCoefs getCoeffcients(unsigned ambiOrder) const;
private:
    string name;
    pFormatCoefs coefficients;
};

struct PFormatConfigs
{
public:
    PFormatConfigs();
    vector<string> getConfigsNames();
    int getMaxPFormatChannels();
    pFormatCoefs getCoefficients(string name, unsigned ambiOrder);
    
private:
    void constructConfigs();
    vector<shared_ptr<PFormatConfig>> configs;
    shared_ptr<PFormatConfig> getConfig(string configName);
};
