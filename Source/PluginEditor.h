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
class WavetableSynthesisTestAudioProcessorEditor  : public juce::AudioProcessorEditor,
    public juce::Slider::Listener,
    public juce::ComboBox::Listener
{
public:
    WavetableSynthesisTestAudioProcessorEditor (WavetableSynthesisTestAudioProcessor&);
    ~WavetableSynthesisTestAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void sliderValueChanged(juce::Slider* slider) override;

    void comboBoxChanged(juce::ComboBox* comboBox) override;


private:
    juce::Font textFont{ 15.0f };
    

    juce::ComboBox* wavetableDropDowns = new juce::ComboBox[5];

    juce::ScopedPointer<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveSelectionTree;

    juce::Label dropDownLabelOne{ {}, "Slot One" };
    juce::Label dropDownLabelTwo{ {}, "Slot Two" };
    juce::Label dropDownLabelThree{ {}, "Slot Three" };
    juce::Label dropDownLabelFour{ {}, "Slot Four" };
    juce::Label dropDownLabelFive{ {}, "Slot Five" };
    juce::Label* dropDownLabels[5] = { &dropDownLabelOne, &dropDownLabelTwo, &dropDownLabelThree, &dropDownLabelFour, &dropDownLabelFive };

    juce::Slider wavescanningSlider;
    
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> wavescanTree;


    juce::Label envelopeLabel{ {}, "Envelope" };

    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    WavetableSynthesisTestAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavetableSynthesisTestAudioProcessorEditor)
};
