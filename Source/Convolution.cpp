/*
  ==============================================================================

    Convolution.cpp
    Created: 8 May 2021 3:42:54pm
    Author:  Haydon Cardew

  ==============================================================================
*/

#include "Convolution.h"

MultiChannelConvolution::MultiChannelConvolution()
{
    
}

void MultiChannelConvolution::setNumberOfChannels(unsigned nChannels)
{
    convolution.resize(nChannels, make_shared<Convolution>());
}

void MultiChannelConvolution::prepare(double sampleRate, unsigned int maximumBlocksize)
{
    processSpec.maximumBlockSize = maximumBlocksize;
    processSpec.sampleRate = sampleRate;
    processSpec.numChannels = 1;
    for (auto & conv : convolution)
    {
        conv->prepare(processSpec);
    }
}

void MultiChannelConvolution::reset()
{
    for (auto & conv : convolution)
    {
        conv->reset();
    }
}

void MultiChannelConvolution::process(vector<vector<float>>& audio, const unsigned nSamples)
{
    assert(audio.size() == convolution.size());
    assert(nSamples <= processSpec.maximumBlockSize);
    assert(audio[0].size() >= nSamples);
    
    for ( int i = 0; i < audio.size(); ++i)
    {
        float* const ptr = audio[i].data();
        AudioBlock<float> block (&ptr, 1, nSamples);
        convolution[i]->process(ProcessContextReplacing<float>(block));
    }
}
