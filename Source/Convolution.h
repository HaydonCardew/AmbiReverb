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
using namespace juce::dsp;

class MultiChannelConvolution
{
public:
    MultiChannelConvolution();
    void setNumberOfChannels(unsigned nChannels);
    void prepare(double sampleRate, unsigned int maximumBlcoksize);
    void process(vector<vector<float>>& audio, const unsigned nSamples);
    void reset();
private:
    vector<shared_ptr<Convolution>> convolution;
    ProcessSpec processSpec;
};
