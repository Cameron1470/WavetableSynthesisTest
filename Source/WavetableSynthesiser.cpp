/*
  ==============================================================================

    WavetableSynthesiser.cpp
    Created: 6 Apr 2021 1:49:34pm
    Author:  csmit

  ==============================================================================
*/

#include "WavetableSynthesiser.h"
#include <JuceHeader.h>

WavetableSynthVoice::WavetableSynthVoice() :
    slotOne(BinaryData::ESW_Analog__Moog_Square_01__wav, BinaryData::ESW_Analog__Moog_Square_01__wavSize, getSampleRate()),
    slotTwo(BinaryData::ESW_Analog__Moog_Triangle_wav, BinaryData::ESW_Analog__Moog_Triangle_wavSize, getSampleRate())
{
    // before processing starts, the wavetables are set
    //slotOne.setWavetable();
    //slotTwo.setWavetable();

    env.setSampleRate(getSampleRate());

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

    float freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);

    // variable for selecting the current wavetable dependent on required frequency
    int currentWavetable = 0;

    // find which wavetable to use, starting at midi note number 19 and incrementing per octave
    while (midiNoteNumber >= (19 + 12 * currentWavetable))
    {
        currentWavetable++;
    }

    wtOscillatorOne.clear();
    wtOscillatorTwo.clear();

    // creating a new WavetableOscillator class from the the AudioSampleBuffer
    auto* oscillatorSlotOne = new WavetableOscillator(slotOne.getAntialiasedWavetable(currentWavetable));
    auto* oscillatorSlotTwo = new WavetableOscillator(slotTwo.getAntialiasedWavetable(currentWavetable));

    // setting the frequency and sample rate in this class instance
    oscillatorSlotOne->setFrequency(freq, getSampleRate());
    oscillatorSlotTwo->setFrequency(freq, getSampleRate());

    // adding to the WavetableOscillator array in private variables
    wtOscillatorOne.add(oscillatorSlotOne);
    wtOscillatorTwo.add(oscillatorSlotTwo);

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

            // creating a sample float, using getNextSample function of the WavetableOscillator class 
            auto currentSample = ((oscillatorSlotOne->getNextSample() * (1 - wavescanBal)) + (oscillatorSlotTwo->getNextSample() * wavescanBal)) * gain * envVal;
            

            // for each channel, write the currentSample float to the output
            for (int chan = 0; chan < outputBuffer.getNumChannels(); chan++)
            {
                // The output sample is scaled by 0.1 so that it is not too loud by default
                outputBuffer.addSample(chan, sampleIndex, currentSample * 0.1);
            }

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