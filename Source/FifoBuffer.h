/*
  ==============================================================================

    FastAudioBuffer.h
    Created: 19 Sep 2020 12:28:52am
    Author:  Haydon Cardew

  ==============================================================================
*/
#pragma once
#include "AudioChunk.h"
#include <vector>

using namespace std;
 
class FifoBuffer
{
public:
    FifoBuffer();
    FifoBuffer(unsigned nChannels, unsigned size);
    void resize(unsigned nChannels, unsigned size);
    unsigned capacity(); // this may be a mistake, vector::capacity?
    void clear();
    bool empty();
    unsigned size();
    bool full();
    // these will read/write silence if less channels are supplied than in the buffer..  maybe okay
    void write(const AudioChunk& data, unsigned nChannels, unsigned nSamplesToWrite, unsigned nSamplesToOverlap=0);
    void write(const float** data, unsigned nChannels, unsigned nSamplesToWrite, unsigned nSamplesToOverlap=0);
    
    void read(AudioChunk& data, unsigned nChannels, unsigned nSamplesToRead, unsigned nSamplesToClear);
    void read(float** data, unsigned nChannels, unsigned nSamplesToRead, unsigned nSamplesToClear);
    
private:
    vector<vector<float>> buffer; // just inherit!
    unsigned head;
    unsigned tail;
    unsigned bufferSize;
    unsigned mask;
};
