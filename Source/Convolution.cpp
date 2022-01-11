#include "Convolution.h"

BFormatConvolution::BFormatConvolution(unsigned order, unsigned maxOrder) : activeChannels(pow(order+1, 2)), maxOrder(maxOrder)
{
    Convolution::NonUniform headSize;
    headSize.headSizeInSamples = 1024;
    convolution.clear();
    const unsigned nChannels = pow(maxOrder+1, 2);
    for (int i = 0; i < nChannels; ++i)
    {
        convolution.push_back(make_shared<Convolution>(headSize));
    }
}

void BFormatConvolution::setOrder(unsigned ambiOrder)
{
    assert(ambiOrder <= maxOrder);
    activeChannels = pow(ambiOrder+1, 2);
    reset();
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

void BFormatConvolution::loadImpulseResponse(ImpulseResponse&& audio, bool normalise)
{
    assert(audio.getNumChannels() == activeChannels);
    Convolution::Normalise norm = normalise ? Convolution::Normalise::yes : Convolution::Normalise::no;
    float** ptrs = audio.getArrayOfWritePointers();
    for ( int i = 0; i < activeChannels; ++i)
    {
        convolution[i]->loadImpulseResponse(AudioBuffer<float>(ptrs + i, 1, 0, audio.getNumSamples()), audio.getSampleRate(), Convolution::Stereo::no, Convolution::Trim::yes, norm);// trim needed?
    }
}

// Mono Input, BFormat Output
void BFormatConvolution::process(vector<float>& input, vector<vector<float>>& output, const unsigned nSamples)
{
    assert(output.size() >= activeChannels);
    assert(nSamples == processSpec.maximumBlockSize);
    assert(input.size() == nSamples);

    for ( int i = 0; i < activeChannels; ++i)
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
