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

//typedef vector<vector<float>> Audio;

class AudioChunk : public vector<vector<float>>
{
public:
    AudioChunk() {};
    AudioChunk(unsigned nChannels, unsigned nSamples) : vector<vector<float>>(nChannels, vector<float>(nSamples, 0.f)) {};
    
    void multiply(vector<vector<float>>& output, const vector<vector<float>>& coefs)
    {
        const vector<vector<float>>& audio = *this;
        
        const int nSamples = audio[0].size();
        const int nInputChannels = audio.size();
        const int nOutputChannels = coefs.size();
        
        assert(nInputChannels == coefs[0].size());
        assert(output.size() == nOutputChannels); // could be >= but this is a nice check
        assert(output[0].size() == nSamples); // could be >= but this is a nice check
        
        // audio[channel][sample]
        // coef[output channel][input channel coef]
        // output[channel][sample]
        for (int sample = 0; sample < nSamples; ++sample)
        {
            for (int outputChannel = 0; outputChannel < nOutputChannels; ++outputChannel)
            {
                output[outputChannel][sample] = audio[0][sample] * coefs[outputChannel][0];
                for (int inputChannel = 1; inputChannel < nInputChannels; ++inputChannel)
                {
                    output[outputChannel][sample] += audio[inputChannel][sample] * coefs[outputChannel][inputChannel];
                }
            }
        }
    }
};

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
