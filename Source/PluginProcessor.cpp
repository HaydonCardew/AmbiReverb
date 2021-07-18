/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

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
                       ),
#endif
bFormatChannels(pow(ambiOrder+1, 2)),
valueTree(*this, nullptr, "ValueTree",
{
    make_unique<AudioParameterFloat>(P_FORMAT_SELECTOR_ID, P_FORMAT_SELECTOR_NAME, 0.0, 1.0, 0.0)
})
{
    decodingMatrix = configList.getDecodingCoefs("T Design (4)", ambiOrder);
    bFormatChunk.resize(bFormatChannels, vector<float>(processBlockSize));
    pFormatChunk.resize(configList.getMaxChannels(), vector<float>(processBlockSize));
    transferChunk.resize(bFormatChannels, vector<float>(processBlockSize));
    
    inputBuffer.resize(bFormatChannels, processBlockSize * 2);
    outputBuffer.resize(bFormatChannels, processBlockSize * 2);
    convolution.clear();
    for (int i = 0; i < configList.getMaxChannels(); ++i)
    {
        convolution.push_back(BFormatConvolution(ambiOrder));
    }
}

AmbiReverbAudioProcessor::~AmbiReverbAudioProcessor()
{
}

int AmbiReverbAudioProcessor::requiredNumIrChannels()
{
    return static_cast<int>(decodingMatrix[0].size()) * bFormatChannels;
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
    for (auto & conv : convolution)
    {
        conv.reset();
        conv.prepare(sampleRate, processBlockSize);
    }
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
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::ambisonic(ambiOrder)) // not sure about this
    {
        //return false;
    }
    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void AmbiReverbAudioProcessor::loadImpulseResponse(unique_ptr<juce::AudioFormatReader> reader)
{
    assert(requiredNumIrChannels() == reader->numChannels); // this is checked in the editor
    const long numSamples = reader->lengthInSamples;
    AudioBuffer<float> impulseResponse(reader->numChannels, (int)numSamples);
    reader->read(&impulseResponse, 0, (int)numSamples, 0, true, true);
    bufferTransfer.set(ImpulseResponse(move(impulseResponse), reader->sampleRate));
    // Don't set loadedImpulseResponse here. Set it in audio thread as that's where it is actually loaded
}

bool AmbiReverbAudioProcessor::hasImpulseResponse()
{
    return loadedImpulseResponse;
}

int AmbiReverbAudioProcessor::numberOfBFormatChannels()
{
    return pow(ambiOrder+1, 2);
}

vector<string> AmbiReverbAudioProcessor::getAvailPFormatSelections()
{
    return configList.getConfigsNames();
}

void AmbiReverbAudioProcessor::setPFormatConfig(string config)
{
    // this needs to be thread safe (and more!)
    lock_guard<mutex> guard (processAudioLock);
    decodingMatrix = configList.getDecodingCoefs (config, ambiOrder);
    for (auto & conv : convolution)
    {
        conv.reset();
    }
    loadedImpulseResponse = false;
}

void AmbiReverbAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    bufferTransfer.get ([this] (ImpulseResponse& ir) // ir = bFromatchannels * pFormatChannels.
    {
        //assert(decodingMatrix.size() == convolution.size()); // nope, to save creating more convs I initalise the maximum straight away
        for (int i = 0; i < decodingMatrix.size(); ++i)
        {
            int startChannel = i * bFormatChannels;
            convolution[i].loadImpulseResponse(ir.getSubBuffer(startChannel, startChannel + bFormatChannels), false);
        }
        loadedImpulseResponse = true;
    });
    
    if (getTotalNumInputChannels() != bFormatChannels || getTotalNumOutputChannels() != bFormatChannels || !loadedImpulseResponse)
    {
        return;
    }
    
    inputBuffer.write(buffer.getArrayOfReadPointers(), buffer.getNumSamples());
    
    lock_guard<mutex> guard(processAudioLock); // a lock in audio thread is possibly silly?
    
    if (inputBuffer.size() >= processBlockSize)
    {
        inputBuffer.read(bFormatChunk, processBlockSize, processBlockSize);
        pFormatChunk.zeroSamples();
        //decodingMatrix.multiply(bFormatChunk, pFormatChunk);
        bFormatChunk.multiply(decodingMatrix, pFormatChunk);
        bFormatChunk.zeroSamples(); // probably dont need
        for (int channel = 0; channel < decodingMatrix.size(); ++channel) //don't do it by pFormatChunk channels! set that to max at start
        {
            convolution[channel].process(pFormatChunk[channel], transferChunk, processBlockSize);
            bFormatChunk.add(transferChunk);
        }
        outputBuffer.write(bFormatChunk, processBlockSize);
    }
    
    if (outputBuffer.size() >= buffer.getNumSamples())
    {
        outputBuffer.read(buffer.getArrayOfWritePointers(), buffer.getNumSamples(), buffer.getNumSamples());
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
