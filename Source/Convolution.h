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

class BFormatConvolution
{
public:
    BFormatConvolution(unsigned order);
    void prepare(double sampleRate, unsigned int maximumBlocksize);
    void loadImpulseResponse(BufferWithSampleRate& audio, bool normalise);
    void process(vector<float>& input, vector<vector<float>>& output, const unsigned nSamples);
    void reset();
private:
    vector<shared_ptr<Convolution>> convolution;
    ProcessSpec processSpec;
    vector<float> transferBuffer;
};
