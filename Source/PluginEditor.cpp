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

    //=========================================================================
    // TOP SECTION - WAVESCANNING

    for (int slot = 0; slot < 5; slot++)
    {
        for (int i = 0; i < BinaryData::namedResourceListSize; i++)
        {
            // switch statement for adding headers at specific points as the for loop works thorugh the binary data
            switch (i) {
            case 0:
                wavetableDropDowns[slot].addSectionHeading("Analog");
                break;
            case 37:
                wavetableDropDowns[slot].addSeparator();
                wavetableDropDowns[slot].addSectionHeading("Basics");
                break;
            case 79:
                wavetableDropDowns[slot].addSeparator();
                wavetableDropDowns[slot].addSectionHeading("FM");
                break;
            case 106:
                wavetableDropDowns[slot].addSeparator();
                wavetableDropDowns[slot].addSectionHeading("Metallic");
                break;
            case 118:
                wavetableDropDowns[slot].addSeparator();
                wavetableDropDowns[slot].addSectionHeading("Spectral");
                break;
            case 133:
                wavetableDropDowns[slot].addSeparator();
                wavetableDropDowns[slot].addSectionHeading("Synth One Shots");
                break;
            }
            
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

    wavetableDropDowns[0].addListener(this);
    waveSelectionTree = new juce::AudioProcessorValueTreeState::ComboBoxAttachment(audioProcessor.parameters, "wavetype_one", wavetableDropDowns[0]);

    // setting the wavetable drop down to match what is set up in WavetableSynthesizer initialisation
    //wavetableDropDowns[0].setSelectedId(23);
    wavetableDropDowns[1].setSelectedId(24);
    wavetableDropDowns[2].setSelectedId(10);
    wavetableDropDowns[3].setSelectedId(13);
    wavetableDropDowns[4].setSelectedId(28);

    // add wavescanning slider, set range and style
    addAndMakeVisible(wavescanningSlider);
    wavescanningSlider.setRange(0, 4);
    wavescanningSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

    // add a listener
    wavescanningSlider.addListener(this);

    // setting the slider to control the wavescan parameter in the audio processor
    wavescanTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "wavescan", wavescanningSlider);

    

    

    //=========================================================================
    // BOTTOM LEFT SECTION - ADSR ENVELOPE

    // add label for section, set font and centre text
    addAndMakeVisible(envelopeLabel);
    envelopeLabel.setFont(textFont);
    envelopeLabel.setJustificationType(juce::Justification::centred);

    // add the 4 ADSR slider to the GUI
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(decaySlider);
    addAndMakeVisible(sustainSlider);
    addAndMakeVisible(releaseSlider);
    
    // setting the range and style of the sliders
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


    // set size of window
    setSize (620, 400);
}

WavetableSynthesisTestAudioProcessorEditor::~WavetableSynthesisTestAudioProcessorEditor()
{
}

//==============================================================================
void WavetableSynthesisTestAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colour(139, 176, 168));

    // drawing outer rectangle for wavescan panel
    juce::Rectangle<int> wavescanPanelOut{ 4, 4, 612, 194 };
    juce::Rectangle<int> envPanelOut{ 4, 202, 100, 194 };
    g.setColour(juce::Colour(47, 61, 59));
    g.drawRect(wavescanPanelOut);
    g.fillRect(wavescanPanelOut);
    g.drawRect(envPanelOut);
    g.fillRect(envPanelOut);

    // draw inner rectangle for wavescan panel
    juce::Rectangle<int> wavescanPanel{ 6, 6, 608, 190 };
    juce::Rectangle<int> envPanel{ 6, 204, 96, 190 };
    g.setColour(juce::Colours::darkslategrey);
    g.drawRect(wavescanPanel);
    g.fillRect(wavescanPanel);
    g.drawRect(envPanel);
    g.fillRect(envPanel);
  
    // drawing lines pointing from the Combo Boxes to the the slider
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

    // positioning the label for the envelope panel
    envelopeLabel.setBounds(6, 204, 96, 20);

    // positioning the ADSR sliders
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

void WavetableSynthesisTestAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBox)
{
    if (comboBox == &wavetableDropDowns[0])
    {
        // finding the index in binary data from the combo box
        int binaryIndex = wavetableDropDowns[0].getSelectedItemIndex() - 1;
        
        // get the data name using this index
        const void* data = BinaryData::originalFilenames[binaryIndex];

        // get the data size using this index???
        size_t dataSize;


        // then call the function in corresponding WavescanningSlot instance

    }
}


