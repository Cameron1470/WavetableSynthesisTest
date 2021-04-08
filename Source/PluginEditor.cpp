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
   
    addAndMakeVisible(dropDownLabel);
    dropDownLabel.setFont(textFont);
    
    addAndMakeVisible(wavetableChooserDropDown);

    for (int i = 0; i < BinaryData::namedResourceListSize; i++)
    {
        wavetableChooserDropDown.addItem(BinaryData::originalFilenames[i], i + 1);
    }


    //wavetableChooserDropDown.onChange = [this] { change Audio file function }

    wavetableChooserDropDown.setSelectedId(1);
    
    setSize (500, 400);
}

WavetableSynthesisTestAudioProcessorEditor::~WavetableSynthesisTestAudioProcessorEditor()
{
}

//==============================================================================
void WavetableSynthesisTestAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::darkslategrey);

}

void WavetableSynthesisTestAudioProcessorEditor::resized()
{
    
    dropDownLabel.setBounds(10, 10, 200, 20);
    wavetableChooserDropDown.setBounds(10, 30, 200, 20);
}
