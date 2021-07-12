/*
  ==============================================================================

    AudioChunk.cpp
    Created: 5 Jun 2021 12:43:32am
    Author:  Haydon Cardew

  ==============================================================================
*/

#include "AudioChunk.h"

//AudioChunk::AudioChunk() {};

AudioChunk::AudioChunk(unsigned nChannels, unsigned nSamples) : Matrix(nChannels, nSamples)
{};

int AudioChunk::getNumChannels() const
{
    return numRows();
}

int AudioChunk::getNumSamples() const
{
    return numColumns();
}

void AudioChunk::zeroSamples()
{
    vector<vector<float>>& audio = *this;
    for (int i = 0; i < audio.size(); ++i)
    {
        for (int j = 0; j < audio[0].size(); ++j)
        {
            audio[i][j] = 0.f;
        }
    }
}

void AudioChunk::add(const AudioChunk& input)
{
    vector<vector<float>>& audio = *this;
    assert(input.size() >= audio.size());
    assert(input[0].size() >= audio[0].size());
    for (int i = 0; i < audio.size(); ++i)
    {
        for (int j = 0; j < audio[0].size(); ++j)
        {
            audio[i][j] += input[i][j];
        }
    }
}

// coefs in format coef[output channel][input channel coef]
/*void AudioChunk::multiply(const AudioChunk& input, const vector<vector<float>>& coefs, AudioChunk& output)
{
    const int nSamples = input.getNumSamples();
    const int nInputChannels = input.getNumChannels();
    const int nOutputChannels = output.getNumChannels();
    
    assert(nInputChannels == coefs[0].size());
    assert(coefs.size() == nOutputChannels); // could be >= but this is a nice check
    assert(output.getNumSamples() == nSamples); // could be >= but this is a nice check
    
    for (int sample = 0; sample < nSamples; ++sample)
    {
        for (int outputChannel = 0; outputChannel < nOutputChannels; ++outputChannel)
        {
            output[outputChannel][sample] = input[0][sample] * coefs[outputChannel][0];
            for (int inputChannel = 1; inputChannel < nInputChannels; ++inputChannel)
            {
                output[outputChannel][sample] += input[inputChannel][sample] * coefs[outputChannel][inputChannel];
            }
        }
    }
}*/
