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
    Convolution::NonUniform headSize;
    headSize.headSizeInSamples = 1024;
    convolution.clear();
    for (int i = 0; i < nChannels; ++i)
    {
        convolution.push_back(make_shared<Convolution>(headSize));
    }
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

void MultiChannelConvolution::loadImpulseResponse(BufferWithSampleRate& audio, bool normalise)
{
    // i think the issue is here...
    assert(audio.getNumChannels() == convolution.size());
    Convolution::Normalise norm = normalise ? Convolution::Normalise::yes : Convolution::Normalise::no;
    float** ptrs = audio.getArrayOfWritePointers();
    for ( int i = 0; i < convolution.size(); ++i)
    {
        convolution[i]->loadImpulseResponse(AudioBuffer<float>(ptrs + i, 1, 0, audio.getNumSamples()), audio.getSampleRate(), Convolution::Stereo::no, Convolution::Trim::yes, norm);// trim needed?
    }
}

void MultiChannelConvolution::process(vector<vector<float>>& audio, const unsigned nSamples)
{
    assert(audio.size() == convolution.size());
    assert(nSamples == processSpec.maximumBlockSize);
    assert(audio[0].size() == nSamples);

    for ( int i = 0; i < audio.size(); ++i)
    {
        float* const ptr = audio[i].data();
        AudioBlock<float> block (&ptr, 1, nSamples);
        ProcessContextReplacing<float> context(block);
        convolution[i]->process(context);
    }
}
