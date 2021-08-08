/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AmbiReverbAudioProcessorEditor::AmbiReverbAudioProcessorEditor (AmbiReverbAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    setResizable(false, true);
    startTimer(100);

    createFileSelectButton();
    createPFormatSelectMenu();
    createAmbiOrderSelectMenu();
    
    updateIRLoadedInformation();
    addAndMakeVisible(irLoaded);
    
    addAndMakeVisible(inputChannelCount);
    addAndMakeVisible(outputChannelCount);
    updateChannelCountInformation();
    
    resized();
}

void AmbiReverbAudioProcessorEditor::timerCallback()
{
    updateChannelCountInformation();
    updateIRLoadedInformation();
}

void AmbiReverbAudioProcessorEditor::createPFormatSelectMenu()
{
    pFormatSelectorAttachment = make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.valueTree, P_FORMAT_SELECTOR_ID, pFormatSelector);
    
    vector<string> selections = audioProcessor.getAvailPFormatSelections();
    for (int i = 0; i < selections.size(); ++i)
    {
        pFormatSelector.addItem (selections[i], i+1); // ID's can't be 0
    }
    pFormatSelector.setJustificationType(Justification::centred);
    atomic<float>* selectorValue = audioProcessor.valueTree.getRawParameterValue(P_FORMAT_SELECTOR_ID);
    float selection = (*selectorValue * (selections.size()-1)) + 1; // scale 0->1 to 1->selections.size()
    pFormatSelector.setSelectedId(selection);
    pFormatSelector.onChange = [this] { audioProcessor.setPFormatConfig(pFormatSelector.getSelectedId() - 1); }; // -1 due to ID starting from 1
    addAndMakeVisible(pFormatSelector);
}

void AmbiReverbAudioProcessorEditor::createAmbiOrderSelectMenu()
{
    ambiOrderSelectorAttachment = make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.valueTree, ORDER_SELECTOR_ID, ambiOrderSelector);
    vector<string> orderSelections{"1st Order", "2nd Order", "3rd Order"};
    assert(orderSelections.size() >= audioProcessor.maxAmbiOrder); // change this
    for (int i = 0; i < audioProcessor.maxAmbiOrder; ++i)
    {
        ambiOrderSelector.addItem (orderSelections[i], i+1);
    }
    ambiOrderSelector.setJustificationType(Justification::centred);
    atomic<float>* ambiSelectorValue = audioProcessor.valueTree.getRawParameterValue(ORDER_SELECTOR_ID);
    ambiOrderSelector.setSelectedId(*ambiSelectorValue);
    ambiOrderSelector.onChange = [this] { audioProcessor.updateAmbisonicOrder(); };
    addAndMakeVisible(ambiOrderSelector);
}

void AmbiReverbAudioProcessorEditor::createFileSelectButton()
{
    fileSelectButton.setButtonText("Choose IR");
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
    fileChooser = make_shared<juce::FileChooser>( "Select a file to load...", juce::File::getSpecialLocation(juce::File::userDesktopDirectory), "*.wav");
    fileSelectButton.onClick = [this, setReader]
    {
        fileChooser->launchAsync (juce::FileBrowserComponent::FileChooserFlags::openMode |
                                 juce::FileBrowserComponent::FileChooserFlags::canSelectFiles,
                                 setReader);
    };
    addAndMakeVisible(fileSelectButton);
}

void AmbiReverbAudioProcessorEditor::updateChannelCountInformation()
{
    int required = audioProcessor.numberOfBFormatChannels();
    
    int in = audioProcessor.getTotalNumInputChannels();
    stringstream inputs;
    inputs << "Input Channels: " << in << "/" << required;
    inputChannelCount.setText(inputs.str(), dontSendNotification);
    inputChannelCount.setColour(Label::textColourId, in >= required ? Colours::white : Colours::red);
    
    int out = audioProcessor.getTotalNumOutputChannels();
    stringstream outputs;
    outputs << "Output Channels: " << out << "/" << required;
    outputChannelCount.setText(outputs.str(), dontSendNotification);
    outputChannelCount.setColour(Label::textColourId, out >= required ? Colours::white : Colours::red);
}

void AmbiReverbAudioProcessorEditor::updateIRLoadedInformation()
{
    bool impulseResponseLoaded = audioProcessor.hasImpulseResponse();
    irLoaded.setText(impulseResponseLoaded ? "Impulse response loaded" : "Impulse response not loaded!", dontSendNotification);
    irLoaded.setColour(Label::textColourId, impulseResponseLoaded ? Colours::white : Colours::red);
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
    fileSelectButton.setBoundsRelative (0.1, 0.1, 0.6, 0.2);
    pFormatSelector.setBoundsRelative (0.1, 0.3, 0.6, 0.2);
    ambiOrderSelector.setBoundsRelative (0.1, 0.5, 0.6, 0.2);
    irLoaded.setBoundsRelative (0.1, 0.7, 0.6, 0.1);
    inputChannelCount.setBoundsRelative (0.1, 0.8, 0.6, 0.1);
    outputChannelCount.setBoundsRelative (0.1, 0.9, 0.6, 0.1);
}
