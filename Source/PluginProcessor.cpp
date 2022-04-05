/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PlayerPluginAudioProcessor::PlayerPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
        : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                                 .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                                 .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
)
#endif
{
    formatManager.registerBasicFormats();
}


PlayerPluginAudioProcessor::~PlayerPluginAudioProcessor() {
}

//==============================================================================
const juce::String PlayerPluginAudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool PlayerPluginAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool PlayerPluginAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool PlayerPluginAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double PlayerPluginAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int PlayerPluginAudioProcessor::getNumPrograms() {
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int PlayerPluginAudioProcessor::getCurrentProgram() {
    return 0;
}

void PlayerPluginAudioProcessor::setCurrentProgram(int index) {
}

const juce::String PlayerPluginAudioProcessor::getProgramName(int index) {
    return {};
}

void PlayerPluginAudioProcessor::changeProgramName(int index, const juce::String &newName) {
}

//==============================================================================
void PlayerPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    transport.prepareToPlay(samplesPerBlock, sampleRate);
}

void PlayerPluginAudioProcessor::releaseResources() {
    transport.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations

bool PlayerPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

#endif

void PlayerPluginAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) {
    for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());
    transport.getNextAudioBlock(juce::AudioSourceChannelInfo(buffer));
}

//==============================================================================
bool PlayerPluginAudioProcessor::hasEditor() const {
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *PlayerPluginAudioProcessor::createEditor() {
    return new PlayerPluginAudioProcessorEditor(*this);
}

//==============================================================================
void PlayerPluginAudioProcessor::getStateInformation(juce::MemoryBlock &destData) {
}

void PlayerPluginAudioProcessor::setStateInformation(const void *data, int sizeInBytes) {
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
    return new PlayerPluginAudioProcessor();
}

void PlayerPluginAudioProcessor::loadFileIntoTransport(const juce::File &audioFile) {
    // unload the previous file source and delete it..
    transport.stop();
    transport.setSource(nullptr);
    currentAudioFileSource = nullptr;

    juce::AudioFormatReader *reader = formatManager.createReaderFor(audioFile);
    currentlyLoadedFile = audioFile;

    if (reader != nullptr) {
        currentAudioFileSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

        // ..and plug it into our transport source
        currentAudioFileSource->setLooping(transport.isLooping());
        transport.setSource(
                currentAudioFileSource.get(),
                0,                   // tells it to buffer this many samples ahead
                nullptr,        // this is the background thread to use for reading-ahead
                reader->sampleRate);     // allows for sample rate correction
    }
}
