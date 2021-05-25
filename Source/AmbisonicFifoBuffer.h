/*
  ==============================================================================

    AmbisonicFifoBuffer.h
    Created: 20 May 2021 11:00:17pm
    Author:  Haydon Cardew

  ==============================================================================
*/

#pragma once
#include <vector>
#include "FifoBuffer.h"

class AmbisonicBuffer : public FifoBuffer
{
public:
    AmbisonicBuffer(unsigned order, unsigned size);
    //void readBFormat(Audio& data, unsigned nSamplesToRead, unsigned nSamplesToClear);
    //void writeBFormat();
    // void read(Audio& data, unsigned nSamplesToRead, unsigned nSamplesToClear);
    // void write(T data, unsigned nSamplesToWrite, unsigned nSamplesToOverlap=0);
    
private:
    
};
