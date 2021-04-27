/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "WavetableOscillator.h"
#include <BinaryData.h>
#include "WavetableSynthesiser.h"

//==============================================================================
/**
*/
class WavemorpherSynthesizerAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    WavemorpherSynthesizerAudioProcessor();
    ~WavemorpherSynthesizerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    // SOME PUBLIC VARIABLES, EASILY ACCESSIBLE FOR THE PLUGIN EDITOR CLASS

    /// The wavescanning parameter
    float wavescanParam;

    /// The ADSR parameters for the envelope
    float attackParam;
    float decayParam;
    float sustainParam;
    float releaseParam;

    // variables of current index of the wavetable in BinaryData updated from GUI
    float slotOneIndexGUI;
    float slotTwoIndexGUI;
    float slotThreeIndexGUI;
    float slotFourIndexGUI;
    float slotFiveIndexGUI;

    float roomSizeParam;
    float dampingParam;
    float dryParam;
    float wetParam;


    juce::AudioProcessorValueTreeState parameters;

private:
    /// Main instance of the synthesizer class
    juce::Synthesiser synth;

    /// Number of voices the synthesizer has
    int voiceCount = 10;

    /// Gain used in process block
    float gain = 0.25f;

    /// Variables for the current index of the wavetable in BinaryData
    float slotOneIndexCurrent;
    float slotTwoIndexCurrent;
    float slotThreeIndexCurrent;
    float slotFourIndexCurrent;
    float slotFiveIndexCurrent;

    /// Juce reverb
    juce::Reverb reverb;

    /// Parameters of the reverb
    juce::Reverb::Parameters reverbParams;

    // Juce dsp Chorus
    juce::dsp::Chorus<float> chorus;

    

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavemorpherSynthesizerAudioProcessor)
};
