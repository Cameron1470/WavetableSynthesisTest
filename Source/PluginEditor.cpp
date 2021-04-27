/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WavemorpherSynthesizerAudioProcessorEditor::WavemorpherSynthesizerAudioProcessorEditor(WavemorpherSynthesizerAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{

    //=========================================================================
    // TOP SECTION - WAVESCANNING

    for (int slot = 0; slot < 5; slot++)
    {
        for (int i = 0; i < BinaryData::namedResourceListSize; i++)
        {
            
            // add all items from binary data to the current drop down slot
            wavetableDropDowns[slot].addItem(BinaryData::originalFilenames[i], i + 1);
 
        }       
        
        // add the drop downs and labels to the GUI
        addAndMakeVisible(wavetableDropDowns[slot]);
        addAndMakeVisible(dropDownLabels[slot]);

        // set font and centre text of labels
        dropDownLabels[slot]->setFont(textFont);
        dropDownLabels[slot]->setJustificationType(juce::Justification::centred);


    }

    // add wavescanning slider, set range and style
    addAndMakeVisible(wavescanningSlider);
    wavescanningSlider.setRange(0, 4);
    wavescanningSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);


    // adding listeners to every drop down menu and connecting them to the processor
    wavetableDropDowns[0].addListener(this);
    waveslotOneTree = new juce::AudioProcessorValueTreeState::ComboBoxAttachment(audioProcessor.parameters, "wavetype_one", wavetableDropDowns[0]);
    wavetableDropDowns[1].addListener(this);
    waveslotTwoTree = new juce::AudioProcessorValueTreeState::ComboBoxAttachment(audioProcessor.parameters, "wavetype_two", wavetableDropDowns[1]);
    wavetableDropDowns[2].addListener(this);
    waveslotThreeTree = new juce::AudioProcessorValueTreeState::ComboBoxAttachment(audioProcessor.parameters, "wavetype_three", wavetableDropDowns[2]);
    wavetableDropDowns[3].addListener(this);
    waveslotFourTree = new juce::AudioProcessorValueTreeState::ComboBoxAttachment(audioProcessor.parameters, "wavetype_four", wavetableDropDowns[3]);
    wavetableDropDowns[4].addListener(this);
    waveslotFiveTree = new juce::AudioProcessorValueTreeState::ComboBoxAttachment(audioProcessor.parameters, "wavetype_five", wavetableDropDowns[4]);


    // add a listener to the slider and connect to the processor
    wavescanningSlider.addListener(this);
    wavescanTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "wavescan", wavescanningSlider);

    //=========================================================================
    // MIXER

    // add label for section, set font and centre text
    addAndMakeVisible(mixerLabel);
    mixerLabel.setFont(textFont);
    mixerLabel.setJustificationType(juce::Justification::centred);

    // add corresponding labels for the sliders in this section, set font and centre text
    addAndMakeVisible(wavesynthLabel);
    wavesynthLabel.setFont(labelFont);
    wavesynthLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(sinesynthLabel);
    sinesynthLabel.setFont(labelFont);
    sinesynthLabel.setJustificationType(juce::Justification::centred);

    // add 1st slider for volume of wavetable synthesizer, set range and appearance
    addAndMakeVisible(wavesynthSlider);
    wavesynthSlider.setRange(0, 1);
    wavesynthSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    wavesynthSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    // add 2nd slider for volume of sinusoidal synthesizer, set range and appearance
    addAndMakeVisible(sinesynthSlider);
    sinesynthSlider.setRange(0, 1);
    sinesynthSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    sinesynthSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    // adding listeners to every slider and connecting them to the processor
    wavesynthSlider.addListener(this);
    wavesynthTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "wave_synth", wavesynthSlider);
    sinesynthSlider.addListener(this);
    sinesynthTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "sine_synth", sinesynthSlider);

    //=========================================================================
    // BOTTOM LEFT SECTION - ADSR ENVELOPE

    // add label for section, set font and centre text
    addAndMakeVisible(envelopeLabel);
    envelopeLabel.setFont(textFont);
    envelopeLabel.setJustificationType(juce::Justification::centred);

    // add corresponding labels for the sliders in this section, set font and centre text
    addAndMakeVisible(attackLabel);
    attackLabel.setFont(textFont);
    attackLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(decayLabel);
    decayLabel.setFont(textFont);
    decayLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(sustainLabel);
    sustainLabel.setFont(textFont);
    sustainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(releaseLabel);
    releaseLabel.setFont(textFont);
    releaseLabel.setJustificationType(juce::Justification::centred);

    // add 1st slider for attack time, set range and appearance
    addAndMakeVisible(attackSlider);
    attackSlider.setRange(0, 1);
    attackSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    attackSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    // add 2nd slider for decay time, set range and appearance
    addAndMakeVisible(decaySlider);
    decaySlider.setRange(0, 1);
    decaySlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    decaySlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    // add 3rd slider for sustain level, set range and appearance
    addAndMakeVisible(sustainSlider);
    sustainSlider.setRange(0, 1);
    sustainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    sustainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    // add 4th slider for relase time, set range and appearance
    addAndMakeVisible(releaseSlider);
    releaseSlider.setRange(0, 4);
    releaseSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    releaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    // adding listeners to every slider and connecting them to the processor
    attackSlider.addListener(this);
    attackTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "attack", attackSlider);
    decaySlider.addListener(this);
    decayTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "decay", decaySlider);
    sustainSlider.addListener(this);
    sustainTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "sustain", sustainSlider);
    releaseSlider.addListener(this);
    releaseTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "release", releaseSlider);

    //=========================================================================
    // FILTER

    // add label for section, set font and centre text
    addAndMakeVisible(filterLabel);
    filterLabel.setFont(textFont);
    filterLabel.setJustificationType(juce::Justification::centred);

    // add corresponding labels for the sliders in this section, set font and centre text
    addAndMakeVisible(lfoSelectionLabel);
    lfoSelectionLabel.setFont(labelFont);
    lfoSelectionLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(cutoffLabel);
    cutoffLabel.setFont(labelFont);
    cutoffLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(resonanceLabel);
    resonanceLabel.setFont(labelFont);
    resonanceLabel.setJustificationType(juce::Justification::centred);

    // add 1st slider for cutoff frequency, set range and appearance
    addAndMakeVisible(cutoffSlider);
    cutoffSlider.setRange(100, 20000);
    cutoffSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    cutoffSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    // add 2nd slider for resonance, set range and appearance
    addAndMakeVisible(resonanceSlider);
    resonanceSlider.setRange(0, 1);
    resonanceSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    resonanceSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    // adding listeners to every slider and connecting them to the processor
    cutoffSlider.addListener(this);
    cutoffTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "cutoff", cutoffSlider);
    resonanceSlider.addListener(this);
    resonanceTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "resonance", resonanceSlider);

    //=========================================================================
    // FILTER ENVELOPE

    // add label for section, set font and centre text
    addAndMakeVisible(filterEnvLabel);
    filterEnvLabel.setFont(textFont);
    filterEnvLabel.setJustificationType(juce::Justification::centred);

    // add corresponding labels for the sliders in this section, set font and centre text
    addAndMakeVisible(filterAttackLabel);
    filterAttackLabel.setFont(textFont);
    filterAttackLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(filterDecayLabel);
    filterDecayLabel.setFont(textFont);
    filterDecayLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(filterSustainLabel);
    filterSustainLabel.setFont(textFont);
    filterSustainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(filterReleaseLabel);
    filterReleaseLabel.setFont(textFont);
    filterReleaseLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(filterCutoffAmpLabel);
    filterCutoffAmpLabel.setFont(labelFont);
    filterCutoffAmpLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(filterResonanceAmpLabel);
    filterResonanceAmpLabel.setFont(labelFont);
    filterResonanceAmpLabel.setJustificationType(juce::Justification::centred);

    // add 1st slider for filter attack time, set range and appearance
    addAndMakeVisible(filterAttackSlider);
    filterAttackSlider.setRange(0, 4);
    filterAttackSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    filterAttackSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    // add 2nd slider for filter decay time, set range and appearance
    addAndMakeVisible(filterDecaySlider);
    filterDecaySlider.setRange(0, 4);
    filterDecaySlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    filterDecaySlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    // add 3rd slider for filter sustain level, set range and appearance
    addAndMakeVisible(filterSustainSlider);
    filterSustainSlider.setRange(0, 1);
    filterSustainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    filterSustainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    // add 4th slider for filter release time, set range and appearance
    addAndMakeVisible(filterReleaseSlider);
    filterReleaseSlider.setRange(0, 4);
    filterReleaseSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    filterReleaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    // add 5th slider for filter cutoff envelope amplitude, set range and appearance
    addAndMakeVisible(filterCutoffAmpSlider);
    filterCutoffAmpSlider.setRange(-1, 1);
    filterCutoffAmpSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    filterCutoffAmpSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    // add 5th slider for filter envelop amplitude, set range and appearance
    addAndMakeVisible(filterResonanceAmpSlider);
    filterResonanceAmpSlider.setRange(-1, 1);
    filterResonanceAmpSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    filterResonanceAmpSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    // adding listeners to every slider and connecting them to the processor
    filterAttackSlider.addListener(this);
    filterAttackTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "filter_attack", filterAttackSlider);
    filterDecaySlider.addListener(this);
    filterDecayTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "filter_decay", filterDecaySlider);
    filterSustainSlider.addListener(this);
    filterSustainTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "filter_sustain", filterSustainSlider);
    filterReleaseSlider.addListener(this);
    filterReleaseTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "filter_release", filterReleaseSlider);
    filterCutoffAmpSlider.addListener(this);
    filterCutoffAmpTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "filter_cutoff_amp", filterCutoffAmpSlider);
    filterResonanceAmpSlider.addListener(this);
    filterResonanceAmpTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "filter_resonance_amp", filterResonanceAmpSlider);

    //=========================================================================
    // CHORUS

    // add label for section, set font and centre text
    addAndMakeVisible(chorusLabel);
    chorusLabel.setFont(textFont);
    chorusLabel.setJustificationType(juce::Justification::centred);

    // add corresponding labels for the sliders in this section, set font and centre text
    addAndMakeVisible(chorusDepthLabel);
    chorusDepthLabel.setFont(labelFont);
    chorusDepthLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(chorusMixLabel);
    chorusMixLabel.setFont(labelFont);
    chorusMixLabel.setJustificationType(juce::Justification::centred);

    // add 1st slider for chorus depth, set range and appearance
    addAndMakeVisible(chorusDepthSlider);
    chorusDepthSlider.setRange(0, 1);
    chorusDepthSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    chorusDepthSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    // add 2nd slider for chorus mix level, set range and appearance
    addAndMakeVisible(chorusMixSlider);
    chorusMixSlider.setRange(0, 1);
    chorusMixSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    chorusMixSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    // adding listeners to every slider and connecting them to the processor
    chorusDepthSlider.addListener(this);
    chorusDepthTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "chorus_depth", chorusDepthSlider);
    chorusMixSlider.addListener(this);
    chorusMixTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "chorus_mix", chorusMixSlider);

    //=========================================================================
    // REVERB

    // add label for section, set font and centre text
    addAndMakeVisible(reverbLabel);
    reverbLabel.setFont(textFont);
    reverbLabel.setJustificationType(juce::Justification::centred);

    // add corresponding labels for the sliders in this section, set font and centre text
    addAndMakeVisible(roomSizeLabel);
    roomSizeLabel.setFont(labelFont);
    roomSizeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(dampingLabel);
    dampingLabel.setFont(labelFont);
    dampingLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(dryLabel);
    dryLabel.setFont(labelFont);
    dryLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(wetLabel);
    wetLabel.setFont(labelFont);
    wetLabel.setJustificationType(juce::Justification::centred);

    // add 1st slider for reverb room size, set range and appearance
    addAndMakeVisible(roomSizeSlider);
    roomSizeSlider.setRange(0, 1);
    roomSizeSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    roomSizeSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    // add 2nd slider for reverb damping level, set range and appearance
    addAndMakeVisible(dampingSlider);
    dampingSlider.setRange(0, 1);
    dampingSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    dampingSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    // add 3rd slider for reverb dry level, set range and appearance
    addAndMakeVisible(drySlider);
    drySlider.setRange(0, 1);
    drySlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    drySlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    // add 4th slider for reverb wet level, set range and appearance
    addAndMakeVisible(wetSlider);
    wetSlider.setRange(0, 1);
    wetSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    wetSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    // adding listeners to every slider and connecting them to the processor
    roomSizeSlider.addListener(this);
    roomSizeTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "room_size", roomSizeSlider);
    dampingSlider.addListener(this);
    dampingTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "damping", dampingSlider);
    drySlider.addListener(this);
    dryTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "dry", drySlider);
    wetSlider.addListener(this);
    wetTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "wet", wetSlider);

    //=========================================================================
    // LFO

    // add label for section, set font and centre text
    addAndMakeVisible(lfoLabel);
    lfoLabel.setFont(textFont);
    lfoLabel.setJustificationType(juce::Justification::centred);

    // add corresponding labels for the sliders in this section, set font and centre text
    addAndMakeVisible(lfoSelectionLabel);
    lfoSelectionLabel.setFont(labelFont);
    lfoSelectionLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(lfoFreqLabel);
    lfoFreqLabel.setFont(labelFont);
    lfoFreqLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(lfoAmpLabel);
    lfoAmpLabel.setFont(labelFont);
    lfoAmpLabel.setJustificationType(juce::Justification::centred);

    // add drop down box for choosing lfo shape, set range and appearance
    addAndMakeVisible(lfoSelection);
    lfoSelection.addItem("Sinusoid", 1);
    lfoSelection.addItem("Triangle", 2);
    lfoSelection.addItem("Saw", 3);
    lfoSelection.addItem("Square", 4);
    lfoSelection.setSelectedId(1);

    // add 1st slider for lfo frequency, set range and appearance
    addAndMakeVisible(lfoFreqSlider);
    lfoFreqSlider.setRange(0, 10);
    lfoFreqSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    lfoFreqSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    // add 2nd slider for lfo amplitude, set range and appearance
    addAndMakeVisible(lfoAmpSlider);
    lfoAmpSlider.setRange(0, 4);
    lfoAmpSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    lfoAmpSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    // adding listeners to every slider and connecting them to the processor
    lfoSelection.addListener(this);
    lfoSelectionTree = new juce::AudioProcessorValueTreeState::ComboBoxAttachment(audioProcessor.parameters, "lfo_shape", lfoSelection);
    lfoFreqSlider.addListener(this);
    lfoFreqTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "lfo_freq", lfoFreqSlider);
    lfoAmpSlider.addListener(this);
    lfoAmpTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "lfo_amp", lfoAmpSlider);

    //=========================================================================

    // add name of the plugin to the interface
    addAndMakeVisible(waveMorpherLabel);
    waveMorpherLabel.setFont(juce::Font(24.0f, juce::Font::bold));
    waveMorpherLabel.setJustificationType(juce::Justification::topLeft);
    waveMorpherLabel.setColour(juce::Label::textColourId, juce::Colour(47, 61, 59));

    // set size of window
    setSize (730, 428);
}

WavemorpherSynthesizerAudioProcessorEditor::~WavemorpherSynthesizerAudioProcessorEditor()
{
}

//==============================================================================
void WavemorpherSynthesizerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colour(139, 176, 168));

    // drawing outer rectangles for all section panels
    juce::Rectangle<int> wavescanPanelOut{ 4, 4, 612, 194 };
    juce::Rectangle<int> mixerPanelOut{ 620, 4, 106, 194 };
    juce::Rectangle<int> envPanelOut{ 4, 202, 100, 194 };
    juce::Rectangle<int> filterPanelOut{ 108, 202, 80, 194 };
    juce::Rectangle<int> filterEnvPanelOut{ 192, 202, 170, 194 };
    juce::Rectangle<int> chorusPanelOut{ 366, 202, 80, 194 };
    juce::Rectangle<int> reverbPanelOut{ 450, 202, 136, 194 };
    juce::Rectangle<int> lfoPanelOut{ 590, 202, 136, 194 };
    g.setColour(juce::Colour(47, 61, 59));
    g.drawRect(wavescanPanelOut);
    g.fillRect(wavescanPanelOut);
    g.drawRect(mixerPanelOut);
    g.fillRect(mixerPanelOut);
    g.drawRect(envPanelOut);
    g.fillRect(envPanelOut);
    g.drawRect(filterPanelOut);
    g.fillRect(filterPanelOut);
    g.drawRect(filterEnvPanelOut);
    g.fillRect(filterEnvPanelOut);
    g.drawRect(chorusPanelOut);
    g.fillRect(chorusPanelOut);
    g.drawRect(reverbPanelOut);
    g.fillRect(reverbPanelOut);
    g.drawRect(lfoPanelOut);
    g.fillRect(lfoPanelOut);

    // drawing inner rectangles for all section panels
    juce::Rectangle<int> wavescanPanel{ 6, 6, 608, 190 };
    juce::Rectangle<int> mixerPanel{ 622, 6, 102, 190 };
    juce::Rectangle<int> envPanel{ 6, 204, 96, 190 };
    juce::Rectangle<int> filterPanel{ 110, 204, 76, 190 };
    juce::Rectangle<int> filterEnvPanel{ 194, 204, 166, 190}; 
    juce::Rectangle<int> chorusPanel{ 368, 204, 76, 190 };
    juce::Rectangle<int> reverbPanel{ 452, 204, 132, 190 };
    juce::Rectangle<int> lfoPanel{ 592, 204, 132, 190 };
    g.setColour(juce::Colours::darkslategrey);
    g.drawRect(wavescanPanel);
    g.fillRect(wavescanPanel);
    g.drawRect(mixerPanel);
    g.fillRect(mixerPanel);
    g.drawRect(envPanel);
    g.fillRect(envPanel);
    g.drawRect(filterPanel);
    g.fillRect(filterPanel);
    g.drawRect(filterEnvPanel);
    g.fillRect(filterEnvPanel);
    g.drawRect(chorusPanel);
    g.fillRect(chorusPanel);
    g.drawRect(reverbPanel);
    g.fillRect(reverbPanel);
    g.drawRect(lfoPanel);
    g.fillRect(lfoPanel);
  
    // drawing lines pointing from the Combo Boxes to the the slider
    juce::Rectangle<int> slotOneLine{ 61, 60, 3, 100 };
    juce::Rectangle<int> slotTwoLine{ 185, 120, 3, 40 };
    juce::Rectangle<int> slotThreeLine{ 309, 60, 3, 100 };
    juce::Rectangle<int> slotFourLine{ 435, 120, 3, 40 };
    juce::Rectangle<int> slotFiveLine{ 559, 60, 3, 100 };
    juce::Rectangle<int> bottomLine{ 212,411,504,3 };
    g.setColour(juce::Colours::antiquewhite);
    g.drawRect(slotOneLine);
    g.drawRect(slotTwoLine);
    g.drawRect(slotThreeLine);
    g.drawRect(slotFourLine);
    g.drawRect(slotFiveLine);
    g.setColour(juce::Colours::lightgrey);
    g.fillRect(slotOneLine);
    g.fillRect(slotTwoLine);
    g.fillRect(slotThreeLine);
    g.fillRect(slotFourLine);
    g.fillRect(slotFiveLine);

    // draw decorative line along bottom
    g.setColour(juce::Colours::ghostwhite);
    g.drawRect(bottomLine);
    g.fillRect(bottomLine);


}

void WavemorpherSynthesizerAudioProcessorEditor::resized()
{
    // positioning drop down labels
    dropDownLabelOne.setBounds(10, 10, 180, 20);
    dropDownLabelTwo.setBounds(96, 70, 180, 20);
    dropDownLabelThree.setBounds(220, 10, 180, 20);
    dropDownLabelFour.setBounds(346, 70, 180, 20);
    dropDownLabelFive.setBounds(430, 10, 180, 20);

    // positioning the drop down boxes themselves
    wavetableDropDowns[0].setBounds(10, 30, 180, 20);
    wavetableDropDowns[1].setBounds(96, 90, 180, 20);
    wavetableDropDowns[2].setBounds(220, 30, 180, 20);
    wavetableDropDowns[3].setBounds(346, 90, 180, 20);
    wavetableDropDowns[4].setBounds(430, 30, 180, 20);

    // positioning the wavescan slider
    wavescanningSlider.setBounds(53, 165, 517, 20);

    //=========================================================================

    mixerLabel.setBounds(622, 10, 102, 20);

    wavesynthSlider.setBounds(643, 36, 60, 60);
    wavesynthLabel.setBounds(643, 96, 60, 15);
    sinesynthSlider.setBounds(643, 111, 60, 60);
    sinesynthLabel.setBounds(637, 171, 72, 15);
    
    //=========================================================================

    envelopeLabel.setBounds(6, 204, 96, 20);

    attackSlider.setBounds(10, 234, 21, 130);
    attackLabel.setBounds(10, 370, 21, 20);
    decaySlider.setBounds(32, 234, 21, 130);
    decayLabel.setBounds(32, 370, 21, 20);
    sustainSlider.setBounds(54, 234, 21, 130);
    sustainLabel.setBounds(54, 370, 21, 20);
    releaseSlider.setBounds(76, 234, 21, 130);
    releaseLabel.setBounds(76, 370, 21, 20);

    //=========================================================================

    filterLabel.setBounds(110, 204, 76, 20);

    cutoffSlider.setBounds(118, 234, 60, 60);
    cutoffLabel.setBounds(118, 294, 60, 15);
    resonanceSlider.setBounds(118, 309, 60, 60);
    resonanceLabel.setBounds(118, 369, 60, 15);

    //=========================================================================

    filterEnvLabel.setBounds(194, 204, 166, 20);

    filterAttackSlider.setBounds(198, 234, 21, 130);
    filterAttackLabel.setBounds(198, 370, 21, 20);
    filterDecaySlider.setBounds(220, 234, 21, 130);
    filterDecayLabel.setBounds(220, 370, 21, 20);
    filterSustainSlider.setBounds(242, 234, 21, 130);
    filterSustainLabel.setBounds(242, 370, 21, 20);
    filterReleaseSlider.setBounds(264, 234, 21, 130);
    filterReleaseLabel.setBounds(264, 370, 21, 20);

    filterCutoffAmpSlider.setBounds(293, 234, 60, 60);
    filterCutoffAmpLabel.setBounds(293, 294, 60, 15);
    filterResonanceAmpSlider.setBounds(293, 309, 60, 60);
    filterResonanceAmpLabel.setBounds(293, 369, 60, 15);

    //=========================================================================

    chorusLabel.setBounds(368, 204, 76, 20);

    chorusDepthSlider.setBounds(376, 234, 60, 60);
    chorusDepthLabel.setBounds(376, 294, 60, 15);
    chorusMixSlider.setBounds(376, 309, 60, 60);
    chorusMixLabel.setBounds(376, 369, 60, 15);

    //=========================================================================

    reverbLabel.setBounds(452, 204, 132, 20);

    roomSizeSlider.setBounds(458, 234, 60, 60);
    roomSizeLabel.setBounds(458, 294, 60, 15);
    drySlider.setBounds(458, 309, 60, 60);
    dryLabel.setBounds(458, 369, 60, 15);
    dampingSlider.setBounds(518, 234, 60, 60);
    dampingLabel.setBounds(518, 294, 60, 15);
    wetSlider.setBounds(518, 309, 60, 60);
    wetLabel.setBounds(518, 369, 60, 15);

    //=========================================================================

    lfoLabel.setBounds(592, 204, 132, 20);

    lfoSelection.setBounds(608, 259, 100, 20);
    lfoSelectionLabel.setBounds(608, 294, 100, 15);
   
    lfoFreqSlider.setBounds(598, 309, 60, 60);
    lfoFreqLabel.setBounds(598, 369, 60, 15);
    lfoAmpSlider.setBounds(658, 309, 60, 60);
    lfoAmpLabel.setBounds(658, 369, 60, 15);

    //=========================================================================

    waveMorpherLabel.setBounds(4, 400, 200, 28);

}

void WavemorpherSynthesizerAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &wavescanningSlider)
    {
        audioProcessor.wavescanParam = wavescanningSlider.getValue();
    }

    if (slider == &attackSlider)
    {
        audioProcessor.attackParam = attackSlider.getValue();
    }

    if (slider == &decaySlider)
    {
        audioProcessor.decayParam = decaySlider.getValue();
    }

    if (slider == &sustainSlider)
    {
        audioProcessor.sustainParam = sustainSlider.getValue();
    }

    if (slider == &releaseSlider)
    {
        audioProcessor.releaseParam = releaseSlider.getValue();
    }
}

void WavemorpherSynthesizerAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBox)
{
 
    if (comboBox == &wavetableDropDowns[0])
    {
        // finding the index in binary data from the combo box and setting the variable in the audio processor
        int binaryIndex = wavetableDropDowns[0].getSelectedItemIndex() - 1;
        audioProcessor.slotOneIndexGUI = binaryIndex;
    }

    if (comboBox == &wavetableDropDowns[1])
    {
        // finding the index in binary data from the combo box and setting the variable in the audio processor
        int binaryIndex = wavetableDropDowns[1].getSelectedItemIndex() - 1;
        audioProcessor.slotTwoIndexGUI = binaryIndex;
    }

    if (comboBox == &wavetableDropDowns[2])
    {
        // finding the index in binary data from the combo box and setting the variable in the audio processor
        int binaryIndex = wavetableDropDowns[2].getSelectedItemIndex() - 1;
        audioProcessor.slotThreeIndexGUI = binaryIndex;
    }

    if (comboBox == &wavetableDropDowns[3])
    {
        // finding the index in binary data from the combo box and setting the variable in the audio processor
        int binaryIndex = wavetableDropDowns[3].getSelectedItemIndex() - 1;
        audioProcessor.slotFourIndexGUI = binaryIndex;
    }

    if (comboBox == &wavetableDropDowns[4])
    {
        // finding the index in binary data from the combo box and setting the variable in the audio processor
        int binaryIndex = wavetableDropDowns[4].getSelectedItemIndex() - 1;
        audioProcessor.slotFiveIndexGUI = binaryIndex;
    }
    
}


