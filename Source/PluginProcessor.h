/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "FifoBuffer.h"
#include "OverlapBuffer.h"

//==============================================================================
/**
*/
class AmbiReverbAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    AmbiReverbAudioProcessor();
    ~AmbiReverbAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    FifoBuffer inputBuffer, outputBuffer;
    vector<vector<float>> transferBuffer;
    vector<OverlapBuffer> overlapBuffer;
    bool blockProcessed;
    const int processBlockSize;
    using WindowType = typename juce::dsp::WindowingFunction<float>::WindowingMethod;
    using WindowFunction = typename juce::dsp::WindowingFunction<float>;
    WindowFunction window;
    juce::dsp::FFT fft;
    unsigned fftOrder = 11;//14; // this controls fftSize and windowLength ~13 for MSc setting
    unsigned fftSize;
    vector<juce::dsp::Complex<float>> fftData, transferData;
    vector<float> shitFilter;
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmbiReverbAudioProcessor)
};
