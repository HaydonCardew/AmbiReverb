/*
  ==============================================================================

    IRBuffer.cpp
    Created: 12 Jul 2021 10:25:20pm
    Author:  Haydon Cardew

  ==============================================================================
*/

#include "ImpulseResponse.h"

//float** ptrs = audio.getArrayOfWritePointers();
//AudioBuffer<float>(ptrs + i, 1, 0, audio.getNumSamples())

ImpulseResponse ImpulseResponse::getSubBuffer(unsigned startChannel, unsigned endChannel)
{
    assert(0 <= startChannel && startChannel < endChannel);
    assert(endChannel <= getNumChannels());
    float** ptrs = getArrayOfWritePointers();
    return ImpulseResponse(AudioBuffer<float>(ptrs + startChannel, endChannel-startChannel, 0, getNumSamples()), getSampleRate());
}
