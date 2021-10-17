#pragma once
#include "ImpulseResponse.h"
#include <JuceHeader.h>
#include <vector>

using namespace std;
using namespace juce;
using namespace juce::dsp;

class BFormatConvolution
{
public:
    BFormatConvolution(unsigned order, unsigned maxOrder);
    void prepare(double sampleRate, unsigned int maximumBlocksize);
    void loadImpulseResponse(ImpulseResponse&& audio, bool normalise);
    void process(vector<float>& input, vector<vector<float>>& output, const unsigned nSamples);
    void reset();
    void setOrder(unsigned ambiOrder);
private:
    vector<shared_ptr<Convolution>> convolution;
    ProcessSpec processSpec;
    vector<float> transferBuffer;
    unsigned activeChannels;
    unsigned maxOrder;
};
