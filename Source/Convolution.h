/*
  ==============================================================================

    Convolution.h
    Created: 8 May 2021 3:42:54pm
    Author:  Haydon Cardew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <vector>

using namespace std;
using namespace juce;
using namespace juce::dsp;

class BufferWithSampleRate : public AudioBuffer<float>
{
public:
    BufferWithSampleRate() = default;

    BufferWithSampleRate (AudioBuffer<float>&& bufferIn, double sampleRateIn)
        : AudioBuffer<float>(std::move (bufferIn)), sampleRate (sampleRateIn) {}
    
    double getSampleRate() { return sampleRate; };
private:
    double sampleRate = 0.0;
};

class MultiChannelConvolution
{
public:
    MultiChannelConvolution();
    void setNumberOfChannels(unsigned nChannels);
    void prepare(double sampleRate, unsigned int maximumBlcoksize);
    //void loadImpulseResponse(vector<vector<float>>& audio, double sampleRate, bool normalise);
    void loadImpulseResponse(BufferWithSampleRate& audio, bool normalise);
    void process(vector<vector<float>>& audio, const unsigned nSamples);
    void reset();
private:
    vector<shared_ptr<Convolution>> convolution;
    //vector<shared_ptr<Gain<float>>> gain;
    ProcessSpec processSpec;
};
