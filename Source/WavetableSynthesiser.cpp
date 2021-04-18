/*
  ==============================================================================

    WavetableSynthesiser.cpp

    Created: 6th April 2021
    Author:  Cameron Smith

  ==============================================================================
*/

#include "WavetableSynthesiser.h"
#include <JuceHeader.h>

WavetableSynthVoice::WavetableSynthVoice() :
    slotOne(BinaryData::ESW_Analog__Moog_Square_01__wav, BinaryData::ESW_Analog__Moog_Square_01__wavSize, getSampleRate()),
    slotTwo(BinaryData::ESW_Analog__Moog_Triangle_wav, BinaryData::ESW_Analog__Moog_Triangle_wavSize, getSampleRate()),
    slotThree(BinaryData::ESW_Analog__JP800_Saw_wav, BinaryData::ESW_Analog__JP800_Saw_wavSize, getSampleRate()),
    slotFour(BinaryData::ESW_Analog__Juno_60_Square_wav, BinaryData::ESW_Analog__Juno_60_Square_wavSize, getSampleRate()),
    slotFive(BinaryData::ESW_Analog__Prophet_Triangle_wav, BinaryData::ESW_Analog__Prophet_Triangle_wavSize, getSampleRate())
{
    // set sample rate of ADSR envelope
    env.setSampleRate(getSampleRate());

    // setting parameters of ADSR envelope
    juce::ADSR::Parameters envParams;
    envParams.attack = 0.1f;
    envParams.decay = 0.25f;
    envParams.sustain = 0.7f;
    envParams.release = 0.25f;
    env.setParameters(envParams);

}

void WavetableSynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/)
{
    playing = true;
    ending = false;

    // store frequency in Hz from the midi note number
    float freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);

    // variable for selecting the current wavetable dependent on required frequency
    int currentWavetable = 0;

    // find which wavetable to use, starting at midi note number 19 and incrementing per octave
    while (midiNoteNumber >= (19 + 12 * currentWavetable))
    {
        currentWavetable++;
    }

    // clear what the wavetables stored in the OwnedArray's
    wtOscillatorOne.clear();
    wtOscillatorTwo.clear();
    wtOscillatorThree.clear();
    wtOscillatorFour.clear();
    wtOscillatorFive.clear();

    // creating a new WavetableOscillator class from the the AudioSampleBuffer
    auto* oscillatorSlotOne = new WavetableOscillator(slotOne.getAntialiasedWavetable(currentWavetable));
    auto* oscillatorSlotTwo = new WavetableOscillator(slotTwo.getAntialiasedWavetable(currentWavetable));
    auto* oscillatorSlotThree = new WavetableOscillator(slotThree.getAntialiasedWavetable(currentWavetable));
    auto* oscillatorSlotFour = new WavetableOscillator(slotFour.getAntialiasedWavetable(currentWavetable));
    auto* oscillatorSlotFive = new WavetableOscillator(slotFive.getAntialiasedWavetable(currentWavetable));


    // setting the frequency and sample rate in this class instance
    oscillatorSlotOne->setFrequency(freq, getSampleRate());
    oscillatorSlotTwo->setFrequency(freq, getSampleRate());
    oscillatorSlotThree->setFrequency(freq, getSampleRate());
    oscillatorSlotFour->setFrequency(freq, getSampleRate());
    oscillatorSlotFive->setFrequency(freq, getSampleRate());

    // adding to the wavetable OwnedArrays in private variables
    wtOscillatorOne.add(oscillatorSlotOne);
    wtOscillatorTwo.add(oscillatorSlotTwo);
    wtOscillatorThree.add(oscillatorSlotThree);
    wtOscillatorFour.add(oscillatorSlotFour);
    wtOscillatorFive.add(oscillatorSlotFive);

    // reset and start envelope
    env.reset();
    env.noteOn();
}

void WavetableSynthVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    env.noteOff();
    ending = true;
}

void WavetableSynthVoice::renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    if (playing) // check to see if this voice should be playing
    {
        // iterate through the necessary number of samples (from startSample up to startSample + numSamples)
        for (int sampleIndex = startSample; sampleIndex < (startSample + numSamples); sampleIndex++)
        {
            float envVal = env.getNextSample();

            // creating a float taken from index 0 of the wtOscillators array
            auto* oscillatorSlotOne = wtOscillatorOne.getUnchecked(0);
            auto* oscillatorSlotTwo = wtOscillatorTwo.getUnchecked(0);
            auto* oscillatorSlotThree = wtOscillatorThree.getUnchecked(0);
            auto* oscillatorSlotFour = wtOscillatorFour.getUnchecked(0);
            auto* oscillatorSlotFive = wtOscillatorFive.getUnchecked(0);

            // creating a sample float, using getNextSample function of the WavetableOscillator class 
            //auto currentSample = ((oscillatorSlotOne->getNextSample() * (1 - wavescanBal)) + (oscillatorSlotTwo->getNextSample() * wavescanBal)) * gain * envVal;
            
            auto slotOneSample = oscillatorSlotOne->getNextSample();
            auto slotTwoSample = oscillatorSlotTwo->getNextSample();
            auto slotThreeSample = oscillatorSlotThree->getNextSample();
            auto slotFourSample = oscillatorSlotFour->getNextSample();
            auto slotFiveSample = oscillatorSlotFive->getNextSample();


            if (wavescanBal <= 1.0)
            {
                currentSample = ((slotOneSample * (1 - wavescanBal)) + (slotTwoSample * wavescanBal)) * gain * envVal;
            }
            else if (wavescanBal <= 2.0)
            {
                float normalizedWavescanVal = wavescanBal - 1.0f;
                currentSample = ((slotTwoSample * (1 - normalizedWavescanVal)) + (slotThreeSample * normalizedWavescanVal)) * gain * envVal;
            }
            else if (wavescanBal <= 3.0)
            {
                float normalizedWavescanVal = wavescanBal - 2.0f;
                currentSample = ((slotThreeSample * (1 - normalizedWavescanVal)) + (slotFourSample * normalizedWavescanVal)) * gain * envVal;
            }
            else if (wavescanBal <= 4.0)
            {
                float normalizedWavescanVal = wavescanBal - 3.0f;
                currentSample = ((slotFourSample * (1 - normalizedWavescanVal)) + (slotFiveSample * normalizedWavescanVal)) * gain * envVal;
            }



            // for each channel, write the currentSample float to the output
            for (int chan = 0; chan < outputBuffer.getNumChannels(); chan++)
            {
                // The output sample is scaled by 0.1 so that it is not too loud by default
                outputBuffer.addSample(chan, sampleIndex, currentSample * 0.1);
            }

            // clear current note if ending and env val is very small
            if (ending)
            {
                if (envVal < 0.0001f)
                {
                    clearCurrentNote();
                    playing = false;

                }
            }
        }
    }
}

void WavetableSynthVoice::setWavescanVal(float _wavescanBal)
{
    wavescanBal = _wavescanBal;
}