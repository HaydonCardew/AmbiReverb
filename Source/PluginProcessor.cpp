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
valueTree(*this, nullptr, "ValueTree",
{
    make_unique<AudioParameterFloat>(P_FORMAT_SELECTOR_ID, P_FORMAT_SELECTOR_NAME, 0.0, 1.0, 0.0),
    make_unique<AudioParameterFloat>(ORDER_SELECTOR_ID, ORDER_SELECTOR_NAME, 1.0, maxAmbiOrder, 1.0)
}), currentConfigIndex(0)
{
    updateAmbisonicOrder(true);
    setPFormatConfig(currentConfigIndex);
    pFormatChunk.resize(configList.getMaxChannels(), vector<float>(processBlockSize));
    int maxBFormatChannels = pow(maxAmbiOrder+1, 2);
    transferChunk.resize(maxBFormatChannels, vector<float>(processBlockSize));
    
    inputBuffer.resize(maxBFormatChannels, processBlockSize * 2);
    outputBuffer.resize(maxBFormatChannels, processBlockSize * 2);
    
    convolution.clear();
    for (int i = 0; i < configList.getMaxChannels(); ++i)
    {
        convolution.push_back(BFormatConvolution(ambiOrder, maxAmbiOrder));
    }
}

void AmbiReverbAudioProcessor::updateAmbisonicOrder(bool forceRefresh)
{
    atomic<float>* newAmbiOrder = valueTree.getRawParameterValue(ORDER_SELECTOR_ID);
    if (ambiOrder == *newAmbiOrder && !forceRefresh)
    {
        return;
    }
    ambiOrder = *newAmbiOrder;
    lock_guard<mutex> guard (processAudioLock);
    decodingMatrix = configList.getDecodingCoefs(currentConfigIndex, ambiOrder);
    bFormatChunk.resize(numberOfBFormatChannels(), vector<float>(processBlockSize)); // allow these to change in realtime?
    for (auto & conv : convolution)
    {
        conv.setOrder(ambiOrder);
    }
}

AmbiReverbAudioProcessor::~AmbiReverbAudioProcessor()
{
}

int AmbiReverbAudioProcessor::requiredNumIrChannels() const
{
    return static_cast<int>(decodingMatrix[0].size()) * numberOfBFormatChannels();
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
    inputBuffer.clear();
    outputBuffer.clear();
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
    /*if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::ambisonic(ambiOrder)) // not sure about this
    {
        return false;
    }*/
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

bool AmbiReverbAudioProcessor::hasImpulseResponse() const
{
    return loadedImpulseResponse;
}

int AmbiReverbAudioProcessor::numberOfBFormatChannels() const
{
    return pow(ambiOrder+1, 2);
}

vector<string> AmbiReverbAudioProcessor::getAvailPFormatSelections() const
{
    return configList.getConfigsNames();
}

void AmbiReverbAudioProcessor::setPFormatConfig(int configIndex)
{
    if (currentConfigIndex == configIndex)
    {
        return;
    }
    loadedImpulseResponse = false;
    currentConfigIndex = configIndex;
    lock_guard<mutex> guard (processAudioLock);
    decodingMatrix = configList.getDecodingCoefs (currentConfigIndex, ambiOrder);
    for (auto & conv : convolution)
    {
        conv.reset();
    }
}

void AmbiReverbAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    bufferTransfer.get ([this] (ImpulseResponse& ir)
    {
        for (int i = 0; i < decodingMatrix.size(); ++i)
        {
            int startChannel = i * numberOfBFormatChannels();
            convolution[i].loadImpulseResponse(ir.getSubBuffer(startChannel, startChannel + numberOfBFormatChannels()), false);
        }
        loadedImpulseResponse = true;
    });
    
    if (getTotalNumInputChannels() < numberOfBFormatChannels() || getTotalNumOutputChannels() < numberOfBFormatChannels() || !loadedImpulseResponse)
    {
        return;
    }
    
    // if there are extra channels then clear them and don't use them
    for (int i = numberOfBFormatChannels(); i < buffer.getNumChannels(); ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
    }
    
    inputBuffer.write(buffer.getArrayOfReadPointers(), buffer.getNumChannels(), buffer.getNumSamples());
    
    lock_guard<mutex> guard(processAudioLock); // a lock in audio thread is possibly stupid?
    
    if (inputBuffer.size() >= processBlockSize)
    {
        inputBuffer.read(bFormatChunk, bFormatChunk.getNumChannels(), processBlockSize, processBlockSize);
        pFormatChunk.zeroSamples();
        decodingMatrix.multiply(bFormatChunk, pFormatChunk);
        bFormatChunk.zeroSamples(); // probably dont need
        for (int channel = 0; channel < decodingMatrix.size(); ++channel) //don't do it by pFormatChunk channels! set that to max at start
        {
            convolution[channel].process(pFormatChunk[channel], transferChunk, processBlockSize);
            bFormatChunk.add(transferChunk);
        }
        outputBuffer.write(bFormatChunk, bFormatChunk.getNumChannels(), processBlockSize);
    }
    
    if (outputBuffer.size() >= buffer.getNumSamples())
    {
        outputBuffer.read(buffer.getArrayOfWritePointers(), buffer.getNumChannels(), buffer.getNumSamples(), buffer.getNumSamples());
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
