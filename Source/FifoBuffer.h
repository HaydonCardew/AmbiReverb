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
    unsigned capacity();
    void clear();
    bool empty();
    unsigned size();
    bool full();
    void write(const AudioChunk& data, unsigned nSamplesToWrite, unsigned nSamplesToOverlap=0);
    void write(const float** data, unsigned nSamplesToWrite, unsigned nSamplesToOverlap=0);
    
    void read(AudioChunk& data, unsigned nSamplesToRead, unsigned nSamplesToClear);
    void read(float** data, unsigned nSamplesToRead, unsigned nSamplesToClear);
private:
    vector<vector<float>> buffer;
    unsigned head;
    unsigned tail;
    unsigned bufferSize;
    unsigned mask;
};
