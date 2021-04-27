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
    
    // Value Tree State object for storing parameters
    juce::AudioProcessorValueTreeState parameters;

private:
    /// Main instance of the synthesizer class
    juce::Synthesiser synth;

    /// Number of voices the synthesizer has
    int voiceCount = 10;

    /// Gain used in process block
    float gain = 0.25f;

    /// Variable for the current index of the wavetable in BinaryData for wavescanning slot 1
    float slotOneIndexCurrent;
    /// Variable for the current index of the wavetable in BinaryData for wavescanning slot 2
    float slotTwoIndexCurrent;
    /// Variable for the current index of the wavetable in BinaryData for wavescanning slot 3
    float slotThreeIndexCurrent;
    /// Variable for the current index of the wavetable in BinaryData for wavescanning slot 4
    float slotFourIndexCurrent;
    /// Variable for the current index of the wavetable in BinaryData for wavescanning slot 5
    float slotFiveIndexCurrent;

    /// Variable of current index of the wavetable in BinaryData for wavescanning slot 1 updated from GUI
    float slotOneIndexGUI;
    /// Variable of current index of the wavetable in BinaryData for wavescanning slot 2 updated from GUI
    float slotTwoIndexGUI;
    /// Variable of current index of the wavetable in BinaryData for wavescanning slot 3 updated from GUI
    float slotThreeIndexGUI;
    /// Variable of current index of the wavetable in BinaryData for wavescanning slot 4 updated from GUI
    float slotFourIndexGUI;
    /// Variable of current index of the wavetable in BinaryData for wavescanning slot 5 updated from GUI
    float slotFiveIndexGUI;

    /// Juce reverb
    juce::Reverb reverb;

    /// Parameters of the reverb
    juce::Reverb::Parameters reverbParams;

    /// Juce DSP Chorus
    juce::dsp::Chorus<float> chorus;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavemorpherSynthesizerAudioProcessor)
};
