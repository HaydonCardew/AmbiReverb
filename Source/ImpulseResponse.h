/*
  ==============================================================================

    IRBuffer.h
    Created: 12 Jul 2021 10:25:20pm
    Author:  Haydon Cardew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <vector>

using namespace std;
using namespace juce;
using namespace juce::dsp;

class ImpulseResponse : public AudioBuffer<float>
{
public:
    ImpulseResponse() = default;

    ImpulseResponse (AudioBuffer<float>&& bufferIn, double sampleRateIn)
        : AudioBuffer<float>(move(bufferIn)), sampleRate (sampleRateIn) {}
    
    ImpulseResponse getSubBuffer(unsigned startChannel, unsigned endChannel);
    
    double getSampleRate() { return sampleRate; };
private:
    double sampleRate = 0.0;
};

class BufferTransfer
{
public:
    void set (ImpulseResponse&& p)
    {
        const SpinLock::ScopedLockType lock (mutex);
        buffer = move (p);
        newBuffer = true;
    }

    // Call `fn` passing the new buffer, if there's one available
    template <typename Fn>
    void get (Fn&& fn)
    {
        const SpinLock::ScopedTryLockType lock (mutex);

        if (lock.isLocked() && newBuffer)
        {
            fn (buffer);
            newBuffer = false;
        }
    }

private:
    ImpulseResponse buffer;
    bool newBuffer = false;
    SpinLock mutex;
};
