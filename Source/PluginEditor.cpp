/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AmbiReverbAudioProcessorEditor::AmbiReverbAudioProcessorEditor (AmbiReverbAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), fileButton("Choose IR"), fileChooser( "Select a file to load...", juce::File::getSpecialLocation(juce::File::userDesktopDirectory), "*.wav")
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    formatManager.registerBasicFormats();
    
    auto setReader = [this] (const juce::FileChooser& chooser)
            {
                const auto result = chooser.getResult();

                if (result != juce::File())
                {
                    //auto readerFactory = new juce::FileAudioFormatReaderFactory (result);
                    //dataModel.setSampleReader (std::unique_ptr<AudioFormatReaderFactory> (readerFactory), &undoManager);
                    juce::AudioFormatReader* reader = formatManager.createReaderFor(result);
                    int nChans = reader->numChannels;
                    int64 length = reader->lengthInSamples;
                    int fs = reader->sampleRate;
                    //bool read (float* const* destChannels, int numDestChannels, int64 startSampleInSource, int numSamplesToRead);
                    //reader->read(&audioProcessor.impulseResponse, 0, length, 0, true, true);
                    audioProcessor.loadImpulseResponse(reader);
                }
            };
    fileButton.onClick = [this, setReader]
    {
        fileChooser.launchAsync (juce::FileBrowserComponent::FileChooserFlags::openMode |
                                 juce::FileBrowserComponent::FileChooserFlags::canSelectFiles,
                                 setReader);
    };
    addAndMakeVisible(fileButton);
}

void AmbiReverbAudioProcessorEditor::selectImpulseResponse()
{
    /*juce::FileChooser fileChooser( "Select a file to load...", juce::File::getSpecialLocation(juce::File::userDesktopDirectory),
        "*.wav");*/
    if (fileChooser.browseForFileToOpen())
    {
        juce::File myFile(fileChooser.getResult());
        
    }
}

AmbiReverbAudioProcessorEditor::~AmbiReverbAudioProcessorEditor()
{
}

//==============================================================================
void AmbiReverbAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void AmbiReverbAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    fileButton.setBoundsRelative(0.1, 0.1, 0.6, 0.2);
}
