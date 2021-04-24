/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <vector>

#define BLOCK_SIZE 2048//16384 //2048

//==============================================================================
AmbiReverbAudioProcessor::AmbiReverbAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), processBlockSize(BLOCK_SIZE), overlapBuffer(2, OverlapBuffer(BLOCK_SIZE, 0.5)), window(BLOCK_SIZE, WindowType::hann), blockProcessed(false), fftSize(BLOCK_SIZE), fft(11)
#endif
{
    transferBuffer.resize(2, vector<float>(processBlockSize));
    inputBuffer.resize(2, processBlockSize * 2);
    outputBuffer.resize(2, processBlockSize * 2);
    outputBuffer.addSilence(processBlockSize/2);
    fftData.resize(processBlockSize);
    transferData.resize(processBlockSize);
    shitFilter.resize(processBlockSize);
    int cutoff = (processBlockSize*0.9);
    for (int i = 0; i < processBlockSize; ++i)
    {
        if (i < cutoff)
        {
            shitFilter[i] = 0;
        }
        else
        {
            shitFilter[i] = 1;
        }
    }
    assert(fftData.size() >= fft.getSize());
}

AmbiReverbAudioProcessor::~AmbiReverbAudioProcessor()
{
}

//==============================================================================
const juce::String AmbiReverbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AmbiReverbAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AmbiReverbAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AmbiReverbAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AmbiReverbAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AmbiReverbAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AmbiReverbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AmbiReverbAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AmbiReverbAudioProcessor::getProgramName (int index)
{
    return {};
}

void AmbiReverbAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AmbiReverbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void AmbiReverbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AmbiReverbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void AmbiReverbAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    //https://docs.juce.com/master/tutorial_looping_audio_sample_buffer.html
    
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    /*for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        
        // ..do something to the data...
    }*/
    inputBuffer.write(vector<const float*>({buffer.getReadPointer(0), buffer.getReadPointer(1)}), buffer.getNumSamples());
    
    if (inputBuffer.size() >= processBlockSize)
    {
        inputBuffer.read(transferBuffer, processBlockSize, processBlockSize/2);
        
        for ( int channel = 0; channel < 2; ++channel)
        {
            for (int sample = 0; sample < processBlockSize; ++sample)
            {
                //transferBuffer[channel][sample] = 0.0;
            }
            window.multiplyWithWindowingTable(transferBuffer[channel].data(), processBlockSize);
            for (int sample = 0; sample < processBlockSize; ++sample)
            {
                transferData[sample] = transferBuffer[channel][sample];
                //transferData[sample] = 0;
                fftData[sample] = 0;
            }
            fft.perform(transferData.data(), fftData.data(), false);
            // process here
            for (int sample = 0; sample < processBlockSize; ++sample)
            {
                fftData[sample] *= shitFilter[sample]; // needs to be the magnitudes multiplied...
                //fftData[sample].real(0);
                //fftData[sample].imag(0);
                //transferData[sample].real(0);
                //transferData[sample].imag(0);
            }
            fft.perform(fftData.data(), transferData.data(), true);
            for (int sample = 0; sample < processBlockSize; ++sample)
            {
                transferBuffer[channel][sample] = transferData[sample].real() / 2; // overlap will double this...
            }
        }
        outputBuffer.write(transferBuffer, processBlockSize, processBlockSize/2); // prefill output buffer
        blockProcessed = true;
    }
    
    if (outputBuffer.size() >= buffer.getNumSamples() && blockProcessed)
    {
        outputBuffer.read(vector<float*>({buffer.getWritePointer(0), buffer.getWritePointer(1)}), buffer.getNumSamples(), buffer.getNumSamples());
    }
}

//==============================================================================
bool AmbiReverbAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AmbiReverbAudioProcessor::createEditor()
{
    return new AmbiReverbAudioProcessorEditor (*this);
}

//==============================================================================
void AmbiReverbAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AmbiReverbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AmbiReverbAudioProcessor();
}
