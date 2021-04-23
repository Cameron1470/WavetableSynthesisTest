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
    slotOneIndexGUI(0),
    slotTwoIndexGUI(2),
    slotThreeIndexGUI(4),
    slotFourIndexGUI(6),
    slotFiveIndexGUI(8),
    slotOneIndexCurrent(0),
    slotTwoIndexCurrent(2),
    slotThreeIndexCurrent(4),
    slotFourIndexCurrent(6),
    slotFiveIndexCurrent(8),
    roomSizeParam(0.5),
    dampingParam(0.5),
    dryParam(0.5),
    wetParam(0.5),
    parameters(*this, nullptr)

{
    //==========================================================================
    // add wavescan parameter to ValueTreeState 
    juce::NormalisableRange<float> wavescanRange(0.0f, 4.0f);
    parameters.createAndAddParameter("wavescan", "Wavescan", "Wavescan", wavescanRange, 2.0f, nullptr, nullptr);
    
    //==========================================================================
    // add wavetable type selection parameter to ValueTreeState
    juce::NormalisableRange<float> wavetableTypeRange(0, 19);
    parameters.createAndAddParameter("wavetype_one", "Wave Type One", "Wavetable One", wavetableTypeRange, 0, nullptr, nullptr);

    parameters.createAndAddParameter("wavetype_two", "Wave Type Two", "Wavetable Two", wavetableTypeRange, 2, nullptr, nullptr);

    parameters.createAndAddParameter("wavetype_three", "Wave Type Three", "Wavetable Three", wavetableTypeRange, 4, nullptr, nullptr);

    parameters.createAndAddParameter("wavetype_four", "Wave Type Four", "Wavetable Four", wavetableTypeRange, 6, nullptr, nullptr);

    parameters.createAndAddParameter("wavetype_five", "Wave Type Five", "Wavetable Five", wavetableTypeRange, 8, nullptr, nullptr);

    //==========================================================================
    // add mixer parameters to value tree state
    juce::NormalisableRange<float> mixerRange(0, 2);

    parameters.createAndAddParameter("wave_synth", "Wavetable Synth", "Wavetable Synth", mixerRange, 1.0f, nullptr, nullptr);

    parameters.createAndAddParameter("sine_synth", "Sine Synth", "Sine Synth", mixerRange, 1.0f, nullptr, nullptr);

    //==========================================================================
    // add ADSR parameters to value tree state
    juce::NormalisableRange<float> attackRange(0.0f, 1.0f);
    parameters.createAndAddParameter("attack", "Attack", "Attack", attackRange, 0.1f, nullptr, nullptr);

    juce::NormalisableRange<float> decayRange(0.0f, 1.0f);
    parameters.createAndAddParameter("decay", "Decay", "Decay", decayRange, 0.5f, nullptr, nullptr);

    juce::NormalisableRange<float> sustainRange(0.0f, 1.0f);
    parameters.createAndAddParameter("sustain", "Sustain", "Sustain", sustainRange, 0.9f, nullptr, nullptr);

    juce::NormalisableRange<float> releaseRange(0.0f, 4.0f);
    parameters.createAndAddParameter("release", "Release", "Release", releaseRange, 0.5f, nullptr, nullptr);
    //==========================================================================
    juce::NormalisableRange<float> cutoffRange(100.0f, 20000.0f);
    parameters.createAndAddParameter("cutoff", "Cutoff", "Cutoff", cutoffRange, 10000.0f, nullptr, nullptr);

    juce::NormalisableRange<float> resonanceRange(0.0f, 1.0f);
    parameters.createAndAddParameter("resonance", "Resonance", "Resonance", resonanceRange, 0.1f, nullptr, nullptr);

    //==========================================================================
    juce::NormalisableRange<float> chorusDepthRange(0.0f, 1.0f);
    parameters.createAndAddParameter("chorus_depth", "Chorus Depth", "Chorus Depth", chorusDepthRange, 0.1f, nullptr, nullptr);

    juce::NormalisableRange<float> chorusMixRange(0.0f, 1.0f);
    parameters.createAndAddParameter("chorus_mix", "Chorus Mix", "Chorus Mix", chorusMixRange, 0.1f, nullptr, nullptr);

    //==========================================================================
    juce::NormalisableRange<float> roomSizeRange(0.0f, 1.0f);
    parameters.createAndAddParameter("room_size", "Room Size", "Room Size", roomSizeRange, 0.5f, nullptr, nullptr);

    juce::NormalisableRange<float> dampingRange(0.0f, 1.0f);
    parameters.createAndAddParameter("damping", "Damping", "Damping", dampingRange, 0.5f, nullptr, nullptr);

    juce::NormalisableRange<float> dryRange(0.0f, 1.0f);
    parameters.createAndAddParameter("dry", "Dry", "Dry", dryRange, 0.6f, nullptr, nullptr);

    juce::NormalisableRange<float> wetRange(0.0f, 1.0f);
    parameters.createAndAddParameter("wet", "Wet", "Wet", wetRange, 0.3f, nullptr, nullptr);

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

    chorus.setDepth(0.5);
    chorus.setMix(0.5);
    ladderFilter.setCutoffFrequencyHz(5000.0f);
    ladderFilter.setResonance(0.1f);

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;

    chorus.prepare(spec);
    chorus.reset();
    ladderFilter.prepare(spec);
    ladderFilter.reset();
    

    reverbParams.dryLevel = 0.5f;
    reverbParams.wetLevel = 0.5f;
    reverbParams.roomSize = 0.5f;
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

        v->setWavetableVolume(parameters.getRawParameterValue("wave_synth"));
        v->setSineVolume(parameters.getRawParameterValue("sine_synth"));
    }

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

        for (int i = 0; i < voiceCount; i++)
        {
            WavetableSynthVoice* v = dynamic_cast<WavetableSynthVoice*>(synth.getVoice(i));

            // now call the setWavetable function for each slot
            // it will only actually do anything if new index =/= current index
            v->updateWavetable(int(slotOneIndexCurrent), 0);
        }
    }

    // do the same for the others
    if (slotTwoIndexGUI != slotTwoIndexCurrent)
    {
        // update binary index parameter
        slotTwoIndexCurrent = slotTwoIndexGUI;

        for (int i = 0; i < voiceCount; i++)
        {
            WavetableSynthVoice* v = dynamic_cast<WavetableSynthVoice*>(synth.getVoice(i));

            // now call the setWavetable function for each slot
            // it will only actually do anything if new index =/= current index
            v->updateWavetable(int(slotTwoIndexCurrent), 1);
        }
    }

    if (slotThreeIndexGUI != slotThreeIndexCurrent)
    {
        // update binary index parameter
        slotThreeIndexCurrent = slotThreeIndexGUI;

        for (int i = 0; i < voiceCount; i++)
        {
            WavetableSynthVoice* v = dynamic_cast<WavetableSynthVoice*>(synth.getVoice(i));

            // now call the setWavetable function for each slot
            // it will only actually do anything if new index =/= current index
            v->updateWavetable(int(slotThreeIndexCurrent), 2);
        }
    }

    if (slotFourIndexGUI != slotFourIndexCurrent)
    {
        // update binary index parameter
        slotFourIndexCurrent = slotFourIndexGUI;

        for (int i = 0; i < voiceCount; i++)
        {
            WavetableSynthVoice* v = dynamic_cast<WavetableSynthVoice*>(synth.getVoice(i));

            // now call the setWavetable function for each slot
            // it will only actually do anything if new index =/= current index
            v->updateWavetable(int(slotFourIndexCurrent), 3);
        }
    }

    if (slotFiveIndexGUI != slotFiveIndexCurrent)
    {
        // update binary index parameter
        slotFiveIndexCurrent = slotFiveIndexGUI;

        for (int i = 0; i < voiceCount; i++)
        {
            WavetableSynthVoice* v = dynamic_cast<WavetableSynthVoice*>(synth.getVoice(i));

            // now call the setWavetable function for each slot
            // it will only actually do anything if new index =/= current index
            v->updateWavetable(int(slotFiveIndexCurrent), 4);
        }
    }

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());


    chorus.setDepth(*parameters.getRawParameterValue("chorus_depth"));
    chorus.setMix(*parameters.getRawParameterValue("chorus_mix"));

    ladderFilter.setCutoffFrequencyHz(*parameters.getRawParameterValue("cutoff"));
    ladderFilter.setResonance(*parameters.getRawParameterValue("resonance"));

    juce::dsp::AudioBlock<float> sampleBlock(buffer);
    chorus.process(juce::dsp::ProcessContextReplacing<float>(sampleBlock));

    ladderFilter.process(juce::dsp::ProcessContextReplacing<float>(sampleBlock));

    reverbParams.roomSize = *parameters.getRawParameterValue("room_size");
    reverbParams.damping = *parameters.getRawParameterValue("damping");
    reverbParams.dryLevel = *parameters.getRawParameterValue("dry");
    reverbParams.wetLevel = *parameters.getRawParameterValue("wet");
    
    reverb.setParameters(reverbParams);

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
