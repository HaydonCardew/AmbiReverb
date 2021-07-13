/*
  ==============================================================================

    AudioChunk.cpp
    Created: 5 Jun 2021 12:43:32am
    Author:  Haydon Cardew

  ==============================================================================
*/

#include "AudioChunk.h"

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
