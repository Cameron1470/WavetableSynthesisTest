/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "WavetableOscillator.h"

//==============================================================================
/**
*/
class WavetableSynthesisTestAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    WavetableSynthesisTestAudioProcessor();
    ~WavetableSynthesisTestAudioProcessor() override;

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
    void createWavetable();

private:
    // for keeping list of avaible audio formats
    juce::AudioFormatManager wtFormatManager;

    // a multi channel buffer containing floating point audio samples - the wavetable itself!
    juce::AudioBuffer<float> wtFileBuffer;

    // an array of WavetableOscillators (not sure about the owned array type, from a juce tutorial)
    juce::OwnedArray<WavetableOscillator> wtOscillators;

    // gain used in process block
    float gain = 0.25f;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavetableSynthesisTestAudioProcessor)
};
