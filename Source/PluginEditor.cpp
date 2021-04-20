/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WavetableSynthesisTestAudioProcessorEditor::WavetableSynthesisTestAudioProcessorEditor(WavetableSynthesisTestAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
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

    // setting the range and style of the sliders
    attackSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    attackSlider.setRange(0, 4);
    attackSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    decaySlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    decaySlider.setRange(0, 4);
    decaySlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    sustainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    sustainSlider.setRange(0, 1);
    sustainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    releaseSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    releaseSlider.setRange(0, 4);
    releaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    // add the 4 ADSR slider to the GUI
    addAndMakeVisible(attackSlider);
    attackSlider.addListener(this);
    attackTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "attack", attackSlider);

    addAndMakeVisible(decaySlider);
    decaySlider.addListener(this);
    decayTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "decay", decaySlider);

    addAndMakeVisible(sustainSlider);
    sustainSlider.addListener(this);
    sustainTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "sustain", sustainSlider);

    addAndMakeVisible(releaseSlider);
    releaseSlider.addListener(this);
    releaseTree = new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "release", releaseSlider);

    
   


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

    attackLabel.setBounds(10, 370, 21, 20);
    decayLabel.setBounds(32, 370, 21, 20);
    sustainLabel.setBounds(54, 370, 21, 20);
    releaseLabel.setBounds(76, 370, 21, 20);


}

void WavetableSynthesisTestAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
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

void WavetableSynthesisTestAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBox)
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


