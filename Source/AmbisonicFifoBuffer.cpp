/*
  ==============================================================================

    AmbisonicFifoBuffer.cpp
    Created: 20 May 2021 11:00:17pm
    Author:  Haydon Cardew

  ==============================================================================
*/

#include "AmbisonicFifoBuffer.h"

AmbisonicBuffer::AmbisonicBuffer(unsigned order, unsigned size) : FifoBuffer(pow(order+1, 2), size)
{
    assert(order == 1); // do the simple things first...
}
