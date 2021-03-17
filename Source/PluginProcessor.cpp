/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WavetableSynthesisTestAudioProcessor::WavetableSynthesisTestAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    // before processing starts, the wavetable is created through the function (defined under process block)
    createWavetable();
}

WavetableSynthesisTestAudioProcessor::~WavetableSynthesisTestAudioProcessor()
{
}

//==============================================================================
const juce::String WavetableSynthesisTestAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WavetableSynthesisTestAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool WavetableSynthesisTestAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool WavetableSynthesisTestAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double WavetableSynthesisTestAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WavetableSynthesisTestAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int WavetableSynthesisTestAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WavetableSynthesisTestAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String WavetableSynthesisTestAudioProcessor::getProgramName (int index)
{
    return {};
}

void WavetableSynthesisTestAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void WavetableSynthesisTestAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // creating a new WavetableOscillator class from the the AudioSampleBuffer
    auto* oscillator = new WavetableOscillator(wtFileBuffer);

    // setting the frequency and sample rate in this class instance
    oscillator->setFrequency(440.0f, (float)sampleRate);

    // adding to the WavetableOscillator array in private variables
    wtOscillators.add(oscillator);
}

void WavetableSynthesisTestAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WavetableSynthesisTestAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void WavetableSynthesisTestAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    //create channel pointers
    float* leftChannel = buffer.getWritePointer(0);
    float* rightChannel = buffer.getWritePointer(1);

    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
        // creating a float taken from index 0 of the wtOscillators array
        auto* oscillator = wtOscillators.getUnchecked(0);
        
        // creating a sample float, using getNextSample function of the WavetableOscillator class 
        auto sample = oscillator->getNextSample() * gain;

        // assigning to channels
        leftChannel[i] = sample;
        rightChannel[i] = sample;
    }

}

void WavetableSynthesisTestAudioProcessor::createWavetable()
{
    // got most of this from here: https://docs.juce.com/master/tutorial_looping_audio_sample_buffer_advanced.html
    // seems quite advanced and I don't really understand some of whats going on here
    
    // allows the program to use basic audio file formats (.wav)
    wtFormatManager.registerBasicFormats();

    // specifying path of wavetable to load, using the JP800 saw file in the wavetables folder
    juce::File wavetableFilePath("C:\JUCE\projects\AudioProgrammingProject\WavetableSynthesisTest\Source\Wavetables\ESW Analog - JP800 Saw.wav");

    // creating an audio format reader from the path
    std::unique_ptr<juce::AudioFormatReader> wtReader(wtFormatManager.createReaderFor(wavetableFilePath));

    // if statement used in juce tutorial, only used when a dialogue box is opened and user clicks cancel? maybe uneccesary?
    if (wtReader.get() != nullptr)
    {
        //setting size of private variable
        wtFileBuffer.setSize((int)wtReader->numChannels, (int)wtReader->lengthInSamples);

        //using the reader to populate AudioSampleBuffer class with the wavetable
        wtReader->read(&wtFileBuffer, 0, (int)wtReader->lengthInSamples, 0, true, true);
    }

}

//==============================================================================
bool WavetableSynthesisTestAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* WavetableSynthesisTestAudioProcessor::createEditor()
{
    return new WavetableSynthesisTestAudioProcessorEditor (*this);
}

//==============================================================================
void WavetableSynthesisTestAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void WavetableSynthesisTestAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WavetableSynthesisTestAudioProcessor();
}
