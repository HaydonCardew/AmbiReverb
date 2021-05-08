/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class AmbiReverbAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    AmbiReverbAudioProcessorEditor (AmbiReverbAudioProcessor&);
    ~AmbiReverbAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AmbiReverbAudioProcessor& audioProcessor;
    void selectImpulseResponse();
    juce::TextButton fileButton;
    juce::FileChooser fileChooser;
    juce::AudioFormatManager formatManager;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmbiReverbAudioProcessorEditor)
};
