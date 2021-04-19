/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WavetableSynthesisTestAudioProcessorEditor::WavetableSynthesisTestAudioProcessorEditor (WavetableSynthesisTestAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    //addAndMakeVisible(dropDownLabel);
    //dropDownLabel.setFont(textFont);

    for (int slot = 0; slot < 5; slot++)
    {
        for (int i = 0; i < BinaryData::namedResourceListSize; i++)
        {
            wavetableDropDowns[slot].addItem(BinaryData::originalFilenames[i], i + 1);
        }
 
        wavetableDropDowns[slot].setSelectedId(1);
        addAndMakeVisible(wavetableDropDowns[slot]);

        addAndMakeVisible(dropDownLabels[slot]);
        dropDownLabels[slot]->setFont(textFont);
        dropDownLabels[slot]->setJustificationType(juce::Justification::centred);
    }

    addAndMakeVisible(envelopeLabel);
    envelopeLabel.setFont(textFont);
    envelopeLabel.setJustificationType(juce::Justification::centred);



    addAndMakeVisible(attackSlider);
    addAndMakeVisible(decaySlider);
    addAndMakeVisible(sustainSlider);
    addAndMakeVisible(releaseSlider);
    
    
    attackSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    attackSlider.setRange(0, 5);
    attackSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    decaySlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    decaySlider.setRange(0, 5);
    decaySlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    sustainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    sustainSlider.setRange(0, 5);
    sustainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    releaseSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    releaseSlider.setRange(0, 5);
    releaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    //wavetableChooserDropDown.onChange = [this] { change Audio file function }

    addAndMakeVisible(wavescanningSlider);
    wavescanningSlider.setRange(0, 4);
    wavescanningSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    wavescanningSlider.addListener(this);

    wavescanTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "wavescan", wavescanningSlider);

    
    setSize (620, 400);
}

WavetableSynthesisTestAudioProcessorEditor::~WavetableSynthesisTestAudioProcessorEditor()
{
}

//==============================================================================
void WavetableSynthesisTestAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll(juce::Colour (66, 89, 76));
    g.fillAll(juce::Colour(139, 176, 168));

    juce::Rectangle<int> wavescanPanelOut{ 4, 4, 612, 194 };
    juce::Rectangle<int> envPanelOut{ 4, 202, 100, 194 };
    g.setColour(juce::Colour(47, 61, 59));
    g.drawRect(wavescanPanelOut);
    g.fillRect(wavescanPanelOut);
    g.drawRect(envPanelOut);
    g.fillRect(envPanelOut);

    juce::Rectangle<int> wavescanPanel{ 6, 6, 608, 190 };
    juce::Rectangle<int> envPanel{ 6, 204, 96, 190 };
    g.setColour(juce::Colours::darkslategrey);
    g.drawRect(wavescanPanel);
    g.fillRect(wavescanPanel);
    g.drawRect(envPanel);
    g.fillRect(envPanel);
  


    juce::Rectangle<int> slotOneLine{ 61, 60, 3, 100 };
    juce::Rectangle<int> slotTwoLine{ 185, 120, 3, 40 };
    juce::Rectangle<int> slotThreeLine{ 309, 60, 3, 100 };
    juce::Rectangle<int> slotFourLine{ 435, 120, 3, 40 };
    juce::Rectangle<int> slotFiveLine{ 559, 60, 3, 100 };
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

}

void WavetableSynthesisTestAudioProcessorEditor::resized()
{
    
    dropDownLabelOne.setBounds(10, 10, 180, 20);
    dropDownLabelTwo.setBounds(96, 70, 180, 20);
    dropDownLabelThree.setBounds(220, 10, 180, 20);
    dropDownLabelFour.setBounds(346, 70, 180, 20);
    dropDownLabelFive.setBounds(430, 10, 180, 20);
    wavetableDropDowns[0].setBounds(10, 30, 180, 20);
    wavetableDropDowns[1].setBounds(96, 90, 180, 20);
    wavetableDropDowns[2].setBounds(220, 30, 180, 20);
    wavetableDropDowns[3].setBounds(346, 90, 180, 20);
    wavetableDropDowns[4].setBounds(430, 30, 180, 20);

    wavescanningSlider.setBounds(53, 165, 517, 20);

    envelopeLabel.setBounds(6, 204, 96, 20);

    attackSlider.setBounds(10, 234, 21, 130);
    decaySlider.setBounds(32, 234, 21, 130);
    sustainSlider.setBounds(54, 234, 21, 130);
    releaseSlider.setBounds(76, 234, 21, 130);


}

void WavetableSynthesisTestAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &wavescanningSlider)
    {
        audioProcessor.wavescanParam = wavescanningSlider.getValue();
    }
}
