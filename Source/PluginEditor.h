/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class WavetableSynthesisTestAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    WavetableSynthesisTestAudioProcessorEditor (WavetableSynthesisTestAudioProcessor&);
    ~WavetableSynthesisTestAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Label dropDownLabel{ {}, "Choose a wavetable file" };
    juce::Font textFont{ 15.0f };
    juce::ComboBox wavetableChooserDropDown;
    
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    WavetableSynthesisTestAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavetableSynthesisTestAudioProcessorEditor)
};
