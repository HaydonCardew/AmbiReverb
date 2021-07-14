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
    startTimer(1000);
    formatManager.registerBasicFormats();
    
    auto setReader = [this] (const juce::FileChooser& chooser)
            {
                const auto result = chooser.getResult();

                if (result != juce::File())
                {
                    unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(result));
                    bool correctNumberOfChannels = (audioProcessor.requiredNumIrChannels() == reader->numChannels);
                    float maxIrLengthSecs = audioProcessor.maxIrLengthMs/1000.f ;
                    bool correctLength = (maxIrLengthSecs >= (reader->lengthInSamples / reader->sampleRate));
                    
                    if (correctNumberOfChannels && correctLength)
                    {
                        audioProcessor.loadImpulseResponse(move(reader));
                    }
                    else
                    {
                        string message = "Error";
                        if (!correctNumberOfChannels)
                        {
                            message = "Incorrect number of channels in impulse response. Required " + to_string(audioProcessor.requiredNumIrChannels());
                        }
                        else if (!correctLength)
                        {
                            stringstream length;
                            length << std::fixed << std::setprecision(1) << maxIrLengthSecs;
                            message = "Impulse response is too long. Max length is " + length.str() + " seconds";
                        }
                        AlertWindow::showMessageBoxAsync(AlertWindow::InfoIcon, "Could not load file", message, "OK");
                    }
                }
            };
    fileButton.onClick = [this, setReader]
    {
        fileChooser.launchAsync (juce::FileBrowserComponent::FileChooserFlags::openMode |
                                 juce::FileBrowserComponent::FileChooserFlags::canSelectFiles,
                                 setReader);
    };
    
    addAndMakeVisible(fileButton);
    
    vector<string> selections = audioProcessor.getAvailPFormatSelections();
    for (int i = 0; i < selections.size(); ++i)
    {
        pFormatSelector.addItem (selections[i], i+1);
    }
     pFormatSelector.onChange = [this] { audioProcessor.setPFormatConfig(pFormatSelector.getText().toStdString()); }; //  this would be safer via IDs
    pFormatSelector.setSelectedId (1);
    addAndMakeVisible(pFormatSelector);
    
    addAndMakeVisible(inputChannelCount);
    addAndMakeVisible(outputChannelCount);
    
    updateChannelCountInformation();
}

void AmbiReverbAudioProcessorEditor::timerCallback()
{
    updateChannelCountInformation();
}

void AmbiReverbAudioProcessorEditor::updateChannelCountInformation()
{
    int required = audioProcessor.numberOfBFormatChannels();
    
    int in = audioProcessor.getTotalNumInputChannels();
    stringstream inputs;
    inputs << "Input Channels: " << in << "/" << required;
    inputChannelCount.setText(inputs.str(), dontSendNotification);
    inputChannelCount.setColour(Label::textColourId, in == required ? Colours::white : Colours::red);
    
    int out = audioProcessor.getTotalNumOutputChannels();
    stringstream outputs;
    outputs << "Output Channels: " << out << "/" << required;
    outputChannelCount.setText(outputs.str(), dontSendNotification);
    outputChannelCount.setColour(Label::textColourId, out == required ? Colours::white : Colours::red);
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
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void AmbiReverbAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    fileButton.setBoundsRelative(0.1, 0.1, 0.6, 0.2);
    pFormatSelector.setBoundsRelative(0.1, 0.3, 0.6, 0.2);
    
    inputChannelCount.setBoundsRelative(0.1, 0.5, 0.6, 0.2);
    outputChannelCount.setBoundsRelative(0.1, 0.7, 0.6, 0.2);
}
