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
    template <typename T>
    void write(T data, unsigned nSamplesToWrite, unsigned nSamplesToOverlap=0);
    template <typename T>
    void read(T data, unsigned nSamplesToRead, unsigned nSamplesToClear);
    /*template <typename T>
    void read(T data, unsigned nSamplesToRead, unsigned nSamplesToClear)
    {
        assert(size() >= nSamplesToRead);
        assert(nSamplesToRead >= nSamplesToClear);
        for (int channel = 0; channel < buffer.size(); channel++)
        {
            int  t = tail;
            for (int sample = 0; sample < nSamplesToRead; ++sample)
            {
                data[channel][sample] = buffer[channel][t++];
                t &= mask;
            }
        }
        tail += nSamplesToClear;
        tail &= mask;
    }*/
    void read(Audio& data, unsigned nSamplesToRead, unsigned nSamplesToClear);
    
    void addSilence(unsigned nSamples);
private:
    Audio buffer;
    unsigned head;
    unsigned tail;
    unsigned bufferSize;
    unsigned mask;
};
