#include "FifoBuffer.h"

FifoBuffer::FifoBuffer() : head(0), tail(0)
{
    
}

FifoBuffer::FifoBuffer(unsigned nChannels, unsigned size) : head(0), tail(0)
{
    resize(nChannels, size);
}

void FifoBuffer::resize(unsigned nChannels, unsigned size)
{
    bufferSize = 1;
    while (bufferSize < size)
    {
        bufferSize <<= 1;
    }
    buffer.clear();
    buffer.resize(nChannels, vector<float>(bufferSize));
    mask = bufferSize - 1;
    head = 0;
    tail = 0;
}

void FifoBuffer::write(const AudioChunk& data, unsigned nChannels, unsigned nSamplesToWrite, unsigned nSamplesToOverlap)
{
    assert(data.size() >= nChannels);
    assert(data[0].size() >= nSamplesToWrite);
    assert(nChannels <= buffer.size());
    assert(nChannels <= data.size());
    vector<const float*> ptrs(nChannels);
    for (int i = 0; i < nChannels; ++i )
    {
        ptrs[i] = data[i].data();
    }
    write(ptrs.data(), nChannels, nSamplesToWrite, nSamplesToOverlap);
}

void FifoBuffer::write(const float** data, unsigned nChannels, unsigned nSamplesToWrite, unsigned nSamplesToOverlap)
{
    assert(nSamplesToWrite >= nSamplesToOverlap);
    assert(size() >= nSamplesToOverlap);
    assert((capacity() - size()) >= nSamplesToWrite);
    assert(nChannels <= buffer.size());
    for (int channel = 0; channel < nChannels; channel++)
    {
        int  h = (head - nSamplesToOverlap) & mask;
        int sample = 0;
        for (; sample < nSamplesToOverlap; ++sample)
        {
            buffer[channel][h++] += data[channel][sample];
            h &= mask;
        }
        for (; sample < nSamplesToWrite; ++sample)
        {
            buffer[channel][h++] = data[channel][sample];
            h &= mask;
        }
    }
    head += (nSamplesToWrite - nSamplesToOverlap);
    head &= mask;
}

void FifoBuffer::read(AudioChunk& data, unsigned nChannels, unsigned nSamplesToRead, unsigned nSamplesToClear)
{
    //assert(data.size() >= buffer.size()); // other way round?
    assert(data[0].size() >= nSamplesToRead);
    assert(nChannels <= buffer.size());
    assert(nChannels <= data.size());
    vector<float*> ptrs(nChannels);
    for (int i = 0; i < nChannels; ++i )
    {
        ptrs[i] = data[i].data();
    }
    read(ptrs.data(), nChannels, nSamplesToRead, nSamplesToClear);
}

void FifoBuffer::read(float** data, unsigned nChannels, unsigned nSamplesToRead, unsigned nSamplesToClear)
{
    assert(size() >= nSamplesToRead);
    assert(nSamplesToRead >= nSamplesToClear);
    for (int channel = 0; channel < nChannels; channel++)
    {
        int  t = tail;
        for (int sample = 0; sample < nSamplesToRead; ++sample)
        {
            data[channel][sample] = buffer[channel][t++];
            t &= mask;
        }
    }
    tail += nSamplesToClear;
    tail &= mask;
}

unsigned FifoBuffer::capacity()
{
    return bufferSize;
}

void FifoBuffer::clear()
{
    head = tail;
}

bool FifoBuffer::empty()
{
    return head == tail;
}

unsigned FifoBuffer::size()
{
    return (head - tail) & mask;
}

bool FifoBuffer::full()
{
    return ((tail + 1) & mask) == head;
}
