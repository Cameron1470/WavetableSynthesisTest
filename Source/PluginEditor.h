/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "WavetableSynthesiser.h"

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
    
    // two font sizes, large and small
    juce::Font textFont{ 15.0f };
    juce::Font labelFont{ 12.0f };
    
    //============================================================================== 
    // WAVESCANNING SECTION
    
    juce::ComboBox* wavetableDropDowns = new juce::ComboBox[5];

    juce::ScopedPointer<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveslotOneTree;
    juce::Label dropDownLabelOne{ {}, "Slot One" };
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveslotTwoTree;
    juce::Label dropDownLabelTwo{ {}, "Slot Two" };
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveslotThreeTree;
    juce::Label dropDownLabelThree{ {}, "Slot Three" };
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveslotFourTree;
    juce::Label dropDownLabelFour{ {}, "Slot Four" };
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveslotFiveTree;
    juce::Label dropDownLabelFive{ {}, "Slot Five" };
    juce::Label* dropDownLabels[5] = { &dropDownLabelOne, &dropDownLabelTwo, &dropDownLabelThree, &dropDownLabelFour, &dropDownLabelFive };

    juce::Slider wavescanningSlider;
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> wavescanTree;

    
    //=================================================================================
    // MIXER SECTION

    juce::Label mixerLabel{ {}, "Mixer" };

    juce::Slider wavesynthSlider;
    juce::Label wavesynthLabel{ {}, "Wavetable" };
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> wavesynthTree;

    juce::Slider sinesynthSlider;
    juce::Label sinesynthLabel{ {}, "Fundamental" };
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> sinesynthTree;

    //=================================================================================
    // AMPLITUDE ENVELOPE
    
    juce::Label envelopeLabel{ {}, "Envelope" };
    
    juce::Slider attackSlider;
    juce::Label attackLabel{ {}, "A" };
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> attackTree;

    juce::Slider decaySlider;
    juce::Label decayLabel{ {}, "D" };
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> decayTree;

    juce::Slider sustainSlider;
    juce::Label sustainLabel{ {}, "S" };
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> sustainTree;

    juce::Slider releaseSlider;
    juce::Label releaseLabel{ {}, "R" };
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> releaseTree;

    //=================================================================================
    // FILTER

    juce::Label filterLabel{ {}, "Filter" };
    
    juce::Slider cutoffSlider;
    juce::Label cutoffLabel{ {}, "Cutoff" };
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffTree;

    juce::Slider resonanceSlider;
    juce::Label resonanceLabel{ {}, "Resonance" };
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> resonanceTree;

    //=================================================================================
    // FILTER ENVELOPE

    juce::Label filterEnvLabel{ {}, "Filter Envelope" };

    juce::Slider filterAttackSlider;
    juce::Label filterAttackLabel{ {}, "A" };
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> filterAttackTree;


    juce::Slider filterDecaySlider;
    juce::Label filterDecayLabel{ {}, "D" };
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> filterDecayTree;

    juce::Slider filterSustainSlider;
    juce::Label filterSustainLabel{ {}, "S" };
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> filterSustainTree;

    juce::Slider filterReleaseSlider;
    juce::Label filterReleaseLabel{ {}, "R" };
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> filterReleaseTree;

    juce::Slider filterEnvAmpSlider;
    juce::Label filterEnvAmpLabel{ {}, "Amplitude" };
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> filterEnvAmpTree;


    //=================================================================================
    // CHORUS

    juce::Label chorusLabel{ {}, "Chorus" };

    juce::Slider chorusDepthSlider;
    juce::Label chorusDepthLabel{ {}, "Depth" };
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> chorusDepthTree;

    juce::Slider chorusMixSlider;
    juce::Label chorusMixLabel{ {}, "Mix" };
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> chorusMixTree;

    //=================================================================================
    // REVERB

    juce::Label reverbLabel{ {}, "Reverb" };

    juce::Slider roomSizeSlider;
    juce::Label roomSizeLabel{ {}, "Room Size" };
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> roomSizeTree;

    juce::Slider dampingSlider;
    juce::Label dampingLabel{ {}, "Damping" };
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> dampingTree;

    juce::Slider drySlider;
    juce::Label dryLabel{ {}, "Dry" };
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> dryTree;

    juce::Slider wetSlider;
    juce::Label wetLabel{ {}, "Wet" };
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> wetTree;

    //=================================================================================
    // LFO

    juce::Label lfoLabel{ {}, "LFO" };

    //=================================================================================

    juce::Label waveMorpherLabel{ {}, "W A V E M O R P H E R" };

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    WavetableSynthesisTestAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavetableSynthesisTestAudioProcessorEditor)
};
