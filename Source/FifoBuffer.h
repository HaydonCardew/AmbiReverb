/*
  ==============================================================================

    FastAudioBuffer.h
    Created: 19 Sep 2020 12:28:52am
    Author:  Haydon Cardew

  ==============================================================================
*/
#pragma once

#include <memory>
#include <vector>
#include <complex>

using namespace std;

typedef vector<vector<float>> Audio;

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
    void write(const Audio& data, unsigned nSamplesToWrite, unsigned nSamplesToOverlap=0);
    void write(const float** data, unsigned nSamplesToWrite, unsigned nSamplesToOverlap=0);
    
    void read(Audio& data, unsigned nSamplesToRead, unsigned nSamplesToClear);
    void read(float** data, unsigned nSamplesToRead, unsigned nSamplesToClear);
    
    void addSilence(unsigned nSamples);
private:
    Audio buffer;
    unsigned head;
    unsigned tail;
    unsigned bufferSize;
    unsigned mask;
};
