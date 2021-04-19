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
                       ),
    
#endif
    wavescanParam(2.0f),
    parameters(*this, nullptr)

{
    juce::NormalisableRange<float> wavescanRange(0.0f, 4.0f);
    parameters.createAndAddParameter("wavescan", "Wavescan", "Wavescan", wavescanRange, 2.0f, nullptr, nullptr);
    
    juce::NormalisableRange<float> wavetableTypeRange(0, 148);
    parameters.createAndAddParameter("wavetype_one", "Wave Type One", "Wavetable One", wavetableTypeRange, 23, nullptr, nullptr);



    // add wavetable synth voices to the synthesiser class
    for (int i = 0; i < voiceCount; i++)
    {
        synth.addVoice(new WavetableSynthVoice());
    }

    synth.addSound(new WavetableSynthSound());
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
    synth.setCurrentPlaybackSampleRate(sampleRate);
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


    for (int i = 0; i < voiceCount; i++)
    {
        WavetableSynthVoice* v = dynamic_cast<WavetableSynthVoice*>(synth.getVoice(i));
        v->setWavescanVal(parameters.getRawParameterValue("wavescan"));


    }
    

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

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
