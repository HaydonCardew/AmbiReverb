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

template <typename T>
void FifoBuffer::write(T data, unsigned nSamplesToWrite, unsigned nSamplesToOverlap)
{
    assert(nSamplesToWrite >= nSamplesToOverlap);
    assert(size() >= nSamplesToOverlap);
    assert((capacity() - size()) >= nSamplesToWrite);
    for (int channel = 0; channel < buffer.size(); channel++)
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
template void FifoBuffer::write<const Audio&>(const Audio&, unsigned, unsigned);
template void FifoBuffer::write<const float**>(const float**, unsigned, unsigned);
//template void FifoBuffer::write<vector<const float*>>(vector<const float*>, unsigned, unsigned);

template <typename T>
void FifoBuffer::read(T data, unsigned nSamplesToRead, unsigned nSamplesToClear)
{
    assert(size() >= nSamplesToRead);
    assert(nSamplesToRead >= nSamplesToClear);
    for (int channel = 0; channel < buffer.size(); channel++)
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
template void FifoBuffer::read<vector<float*>>(vector<float*>, unsigned, unsigned);
template void FifoBuffer::read<float**>(float**, unsigned, unsigned);
//template void FifoBuffer::read< Audio & >(Audio &, unsigned, unsigned);

//template void FifoBuffer::read<vector<vector<float>>&>(vector<vector<float>>&, unsigned, unsigned);

void FifoBuffer::read(Audio& data, unsigned nSamplesToRead, unsigned nSamplesToClear)
{
    assert(size() >= nSamplesToRead);
    assert(nSamplesToRead >= nSamplesToClear);
    for (int channel = 0; channel < buffer.size(); channel++)
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

void FifoBuffer::addSilence(unsigned nSamples)
{
    Audio silence(buffer.size(), vector<float>(nSamples, 0));
    write(silence, nSamples);
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
