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
#include "Matrix.h"
#include <math.h>

using namespace std;

struct MicrophonePosition
{
public:
    // I think angles are couner-clockwise for some stupid reason?
    MicrophonePosition (float azimuth, float elevation) : azimuth(azimuth), elevation(elevation)
    {
        //assert(0.f <= azimuth && azimuth <= 360.f);
        //assert(0.f <= elevation && elevation <= 360.f);
        assert(-360.f <= azimuth && azimuth <= 360.f);
        assert(-360.f <= elevation && elevation <= 360.f);
    };
    // https://w2.mat.ucsb.edu/240/D/notes/Ambisonics.html
    // Return SN3D coefs
    vector<float> getAmbisonicCoefficients(unsigned order) const
    {
        assert(1 <= order && order <= 3); // currently...
        vector<float> coefs(pow(order+1, 2));
        coefs[0] = 1; // W
        coefs[1] = sin(azimuth) * cos(elevation); // Y
        coefs[2] = sin(elevation); // Z
        coefs[3] = cos(azimuth) * cos(elevation); // X
        if (order > 1)
        {
            coefs[4] = sqrt(3.0/2.0) * sin(2*azimuth) * pow(cos(elevation), 2); // V
            coefs[5] = sqrt(3.0/2.0) * sin(azimuth) * sin(2*elevation); // T
            coefs[6] = (3 * pow(sin(elevation), 2) - 1) / 2; // R
            coefs[7] = sqrt(3.0/2.0) * cos(azimuth) * sin(2*elevation); // S
            coefs[8] = sqrt(3.0/2.0) * cos(2*azimuth) * pow(cos(elevation), 2); // U
        }
        if (order > 2)
        {
            coefs[9] = sqrt(5.0/8.0) * sin(3*azimuth) * pow(cos(elevation), 3); // Q
            coefs[10] = sqrt(15.0/2.0) * sin(2*azimuth) * sin(elevation) * pow(cos(elevation), 2); // O
            coefs[11] = sqrt(3.0/8.0) * sin(azimuth) * cos(elevation) * (5 * pow(sin(elevation), 2) - 1); // M
            coefs[12] = sin(elevation) * (5 * pow(sin(elevation), 2) - 3) / 2; // K
            coefs[13] = sqrt(3.0/8.0) * cos(azimuth) * cos(elevation) * (5 * pow(sin(elevation), 2) - 1); // L
            coefs[14] = sqrt(15.0/2.0) * cos(2*azimuth) * sin(elevation) * pow(cos(elevation), 2); // N
            coefs[15] = sqrt(5.0/8.0) * cos(3*azimuth) * pow(cos(elevation), 3); // P
        }
        return coefs;
    };
private:
    const float azimuth;
    const float elevation;
};

struct PFormatConfig
{
public:
    PFormatConfig (string name, const vector<MicrophonePosition> microphonePositions)
    : microphonePositions(microphonePositions), name(name)
    {
        for (auto & position : microphonePositions)
        {
            encodingCoefs.addRow(position.getAmbisonicCoefficients(3));
        }
        decodingCoefs = encodingCoefs.getPseudoInverse();
        //assert(checkCoefsAreInverse());
    }
    
    PFormatConfig (string name, const vector<MicrophonePosition> microphonePositions, const HC::Matrix decodingCoefs)
    : microphonePositions(microphonePositions), name(name), decodingCoefs(decodingCoefs)
    {
        for (auto & position : microphonePositions)
        {
            encodingCoefs.addRow(position.getAmbisonicCoefficients(3));
        }
        //assert(checkCoefsAreInverse());
    }
    
    unsigned getNumChannels()
    {
        return static_cast<unsigned>(microphonePositions.size());
    }
    
    HC::Matrix getDecodingCoefs()
    {
        return decodingCoefs;
    }
    
    HC::Matrix getEncodingCoefs()
    {
        return encodingCoefs;
    }
    
    string getName() const;
private:
    const vector<MicrophonePosition> microphonePositions;
    bool checkCoefsAreInverse ();
    HC::Matrix decodingCoefs;
    HC::Matrix encodingCoefs;
    string name;
};

struct PFormatConfigs
{
public:
    PFormatConfigs();
    int getMaxChannels();
    vector<string> getConfigsNames();
    HC::Matrix getDecodingCoefs(const string name);
    HC::Matrix getDecodingCoefs(const string name, unsigned ambiOrder);
private:
    void constructConfigs();
    vector<PFormatConfig> configs;
};
