/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ImpulseResponse.h"
#include "FifoBuffer.h"
#include "Convolution.h"
#include "PFormatConfig.h"
//==============================================================================
/**
*/

using namespace juce;

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

    void loadImpulseResponse(juce::AudioFormatReader* reader);
    vector<string> getAvailPFormatSelections();
    void setPFormatConfig(string config);
    
    static const int processBlockSize = 2048;
    static const int ambiOrder = 1;
    static const int maxIrLengthMs = 4000;
    int requiredNumIrChannels();
    
    void numChannelsChanged() override;
    
private:
    
    FifoBuffer inputBuffer, outputBuffer;
    AudioChunk bFormatChunk, pFormatChunk, transferChunk;
    
    int bFormatChannels;
    vector<BFormatConvolution> convolution;
    PFormatConfigs configList;
    HC::Matrix decodingMatrix;
    BufferTransfer bufferTransfer;
    
    mutex processAudioLock;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmbiReverbAudioProcessor)
};
