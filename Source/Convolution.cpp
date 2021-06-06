/*
  ==============================================================================

    Convolution.cpp
    Created: 8 May 2021 3:42:54pm
    Author:  Haydon Cardew

  ==============================================================================
*/

#include "Convolution.h"

BFormatConvolution::BFormatConvolution(unsigned order)
{
    Convolution::NonUniform headSize;
    headSize.headSizeInSamples = 1024;
    convolution.clear();
    const unsigned nChannels = pow(order+1, 2);
    for (int i = 0; i < nChannels; ++i)
    {
        convolution.push_back(make_shared<Convolution>(headSize));
    }
}

void BFormatConvolution::prepare(double sampleRate, unsigned int maximumBlockSize)
{
    processSpec.maximumBlockSize = maximumBlockSize;
    processSpec.sampleRate = sampleRate;
    processSpec.numChannels = 1;
    for (auto & conv : convolution)
    {
        conv->prepare(processSpec);
    }
    transferBuffer.resize(maximumBlockSize);
}

void BFormatConvolution::loadImpulseResponse(BufferWithSampleRate& audio, bool normalise)
{
    assert(audio.getNumChannels() == convolution.size());
    Convolution::Normalise norm = normalise ? Convolution::Normalise::yes : Convolution::Normalise::no;
    float** ptrs = audio.getArrayOfWritePointers();
    for ( int i = 0; i < convolution.size(); ++i)
    {
        convolution[i]->loadImpulseResponse(AudioBuffer<float>(ptrs + i, 1, 0, audio.getNumSamples()), audio.getSampleRate(), Convolution::Stereo::no, Convolution::Trim::yes, norm);// trim needed?
    }
}

// Mono Input, BFormat Output
void BFormatConvolution::process(vector<float>& input, vector<vector<float>>& output, const unsigned nSamples)
{
    assert(output.size() == convolution.size());
    assert(nSamples == processSpec.maximumBlockSize);
    assert(input.size() == nSamples);

    for ( int i = 0; i < output.size(); ++i)
    {
        float* const inPtr = input.data();
        AudioBlock<float> inputBlock (&inPtr, 1, nSamples);
        float* const outPtr = output[i].data();
        AudioBlock<float> outputBlock (&outPtr, 1, nSamples);
        ProcessContextNonReplacing<float> context(inputBlock, outputBlock);
        convolution[i]->process(context);
    }
}

void BFormatConvolution::reset()
{
    for (auto & conv : convolution)
    {
        conv->reset();
    }
}
