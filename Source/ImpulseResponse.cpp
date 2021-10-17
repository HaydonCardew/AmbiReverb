#include "ImpulseResponse.h"

ImpulseResponse ImpulseResponse::getSubBuffer(unsigned startChannel, unsigned endChannel)
{
    assert(0 <= startChannel && startChannel < endChannel);
    assert(endChannel <= getNumChannels());
    float** ptrs = getArrayOfWritePointers();
    return ImpulseResponse(AudioBuffer<float>(ptrs + startChannel, endChannel-startChannel, 0, getNumSamples()), getSampleRate());
}
