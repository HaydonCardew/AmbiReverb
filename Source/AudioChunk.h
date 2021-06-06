/*
  ==============================================================================

    AudioChunk.h
    Created: 5 Jun 2021 12:43:32am
    Author:  Haydon Cardew

  ==============================================================================
*/

#pragma once
#include <vector>

using namespace std;

class AudioChunk : public vector<vector<float>>
{
public:
    AudioChunk();
    AudioChunk(unsigned nChannels, unsigned nSamples);
    
    int getNumChannels() const;
    int getNumSamples() const;
    void zeroSamples();
    void add(AudioChunk& input);
    static void multiply(const AudioChunk& input, const vector<vector<float>>& coefs, AudioChunk& output);
};
