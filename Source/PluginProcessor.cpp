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
    attackParam(0.1f),
    decayParam(0.1f),
    sustainParam(0.9f),
    releaseParam(0.5f),
    slotOneIndexGUI(22),
    slotTwoIndexGUI(23),
    slotThreeIndexGUI(9),
    slotFourIndexGUI(12),
    slotFiveIndexGUI(28),
    slotOneIndexCurrent(22),
    slotTwoIndexCurrent(23),
    slotThreeIndexCurrent(9),
    slotFourIndexCurrent(12),
    slotFiveIndexCurrent(28),
    parameters(*this, nullptr)

{
    //==========================================================================
    // add wavescan parameter to ValueTreeState 
    juce::NormalisableRange<float> wavescanRange(0.0f, 4.0f);
    parameters.createAndAddParameter("wavescan", "Wavescan", "Wavescan", wavescanRange, 2.0f, nullptr, nullptr);
    
    //==========================================================================
    // add wavetable type selection parameter to ValueTreeState
    juce::NormalisableRange<float> wavetableTypeRange(0, 148);
    parameters.createAndAddParameter("wavetype_one", "Wave Type One", "Wavetable One", wavetableTypeRange, 22, nullptr, nullptr);

    parameters.createAndAddParameter("wavetype_two", "Wave Type Two", "Wavetable Two", wavetableTypeRange, 23, nullptr, nullptr);

    parameters.createAndAddParameter("wavetype_three", "Wave Type Three", "Wavetable Three", wavetableTypeRange, 9, nullptr, nullptr);

    parameters.createAndAddParameter("wavetype_four", "Wave Type Four", "Wavetable Four", wavetableTypeRange, 12, nullptr, nullptr);

    parameters.createAndAddParameter("wavetype_five", "Wave Type Five", "Wavetable Five", wavetableTypeRange, 28, nullptr, nullptr);



    //==========================================================================
    // add ADSR parameters to value tree state
    juce::NormalisableRange<float> attackRange(0.0f, 4.0f);
    parameters.createAndAddParameter("attack", "Attack", "Attack", attackRange, 0.1f, nullptr, nullptr);

    juce::NormalisableRange<float> decayRange(0.0f, 4.0f);
    parameters.createAndAddParameter("decay", "Decay", "Decay", decayRange, 0.1f, nullptr, nullptr);

    juce::NormalisableRange<float> sustainRange(0.0f, 1.0f);
    parameters.createAndAddParameter("sustain", "Sustain", "Sustain", sustainRange, 0.9f, nullptr, nullptr);

    juce::NormalisableRange<float> releaseRange(0.0f, 4.0f);
    parameters.createAndAddParameter("release", "Release", "Release", releaseRange, 0.5f, nullptr, nullptr);

    //==========================================================================
    // add wavetable synth voices to the synthesiser class
    for (int i = 0; i < voiceCount; i++)
    {
        synth.addVoice(new WavetableSynthVoice());
    }

    // add wavetable synth sound to the synthesiser class
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

    reverbParams.dryLevel = 0.5f;
    reverbParams.wetLevel = 0.5f;
    reverbParams.roomSize = 0.6f;
    reverb.setParameters(reverbParams);
    reverb.reset();

    slotOneIndexCurrent = slotOneIndexGUI;
    slotTwoIndexCurrent = slotTwoIndexGUI;
    slotThreeIndexCurrent = slotThreeIndexGUI;
    slotFourIndexCurrent = slotFourIndexGUI;
    slotFiveIndexCurrent = slotFiveIndexGUI;
    for (int i = 0; i < voiceCount; i++)
    {
        WavetableSynthVoice* v = dynamic_cast<WavetableSynthVoice*>(synth.getVoice(i));
        v->updateWavetable(int(slotOneIndexCurrent), 0);
        v->updateWavetable(int(slotTwoIndexCurrent), 1);
        v->updateWavetable(int(slotThreeIndexCurrent), 2);
        v->updateWavetable(int(slotFourIndexCurrent), 3);
        v->updateWavetable(int(slotFiveIndexCurrent), 4);
    }

    
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
        v->setAttack(parameters.getRawParameterValue("attack"));
        v->setDecay(parameters.getRawParameterValue("decay"));
        v->setSustain(parameters.getRawParameterValue("sustain"));
        v->setRelease(parameters.getRawParameterValue("release"));
        
        //================================================================================

        slotOneIndexGUI = *parameters.getRawParameterValue("wavetype_one");
        slotTwoIndexGUI = *parameters.getRawParameterValue("wavetype_two");
        slotThreeIndexGUI = *parameters.getRawParameterValue("wavetype_three");
        slotFourIndexGUI = *parameters.getRawParameterValue("wavetype_four");
        slotFiveIndexGUI = *parameters.getRawParameterValue("wavetype_five");

        // only call the set wavetable function if something has been changed in the indices
        if (slotOneIndexGUI != slotOneIndexCurrent)
        {
            // update binary index parameter
            slotOneIndexCurrent = slotOneIndexGUI;
            
            // now call the setWavetable function for each slot
            // it will only actually do anything if new index =/= current index
            v->updateWavetable(int(slotOneIndexCurrent), 0);

        }
        else if (slotTwoIndexGUI != slotTwoIndexCurrent)
        {
            slotTwoIndexCurrent = slotTwoIndexGUI;
            v->updateWavetable(int(slotTwoIndexCurrent), 1);
        }
        else if (slotThreeIndexGUI != slotThreeIndexCurrent)
        {
            slotThreeIndexCurrent = slotThreeIndexGUI;
            v->updateWavetable(int(slotThreeIndexCurrent), 2);
        }
        else if (slotFourIndexGUI != slotFourIndexCurrent)
        {
            slotFourIndexCurrent = slotFourIndexGUI;
            v->updateWavetable(int(slotFourIndexCurrent), 3);
        }
        else if (slotFiveIndexGUI != slotFiveIndexCurrent)
        {
            slotFiveIndexCurrent = slotFiveIndexGUI;
            v->updateWavetable(int(slotFiveIndexCurrent), 4);
        }

        //================================================================================
 
    }
    

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    reverb.processStereo(buffer.getWritePointer(0), buffer.getWritePointer(1), buffer.getNumSamples());


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



//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WavetableSynthesisTestAudioProcessor();
}
