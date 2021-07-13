/*
  ==============================================================================

    AudioChunk.h
    Created: 5 Jun 2021 12:43:32am
    Author:  Haydon Cardew

  ==============================================================================
*/

#pragma once
#include <vector>
#include "Matrix.h"

using namespace std;

class AudioChunk : public HC::Matrix
{
public:
    using Matrix::Matrix;
    AudioChunk(unsigned nChannels, unsigned nSamples);
    
    int getNumChannels() const;
    int getNumSamples() const;
    void zeroSamples();
    void add(const AudioChunk& input);
};
