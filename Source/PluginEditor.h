#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class AmbiReverbAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::Timer
{
public:
    AmbiReverbAudioProcessorEditor (AmbiReverbAudioProcessor&);
    ~AmbiReverbAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    AmbiReverbAudioProcessor& audioProcessor;
    
    void createFileSelectButton();
    juce::TextButton fileSelectButton;
    shared_ptr<juce::FileChooser> fileChooser;
    juce::AudioFormatManager formatManager;
    
    juce::Label inputChannelCount;
    juce::Label outputChannelCount;
    juce::Label irLoaded;
    
    void createPFormatSelectMenu();
    void createAmbiOrderSelectMenu();
    juce::ComboBox pFormatSelector, ambiOrderSelector;
    unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> pFormatSelectorAttachment, ambiOrderSelectorAttachment;
    
    void updateChannelCountInformation();
    void updateIRLoadedInformation();
    
    void timerCallback() override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmbiReverbAudioProcessorEditor)
};
