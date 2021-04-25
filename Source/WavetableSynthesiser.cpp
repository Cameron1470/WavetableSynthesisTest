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
    slotOne(getSampleRate()),
    slotTwo(getSampleRate()),
    slotThree(getSampleRate()),
    slotFour(getSampleRate()),
    slotFive(getSampleRate())
    


{
<<<<<<< HEAD
=======
    // set sample rate of ADSR envelope
    env.setSampleRate(getSampleRate());

    
>>>>>>> parent of 7c7f331 (issues with filter on synth voices)

    
}

void WavetableSynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/)
{

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


    fundamentalOsc.setFrequency(freq);

    // start volume envelope
    adsr.noteOn();

    // start filter envelope
    filterAdsr.noteOn();




}

void WavetableSynthVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    adsr.noteOff();
    filterAdsr.noteOff();
    
    if (!allowTailOff || !adsr.isActive())
    {
        clearCurrentNote();
    }
        
}

void WavetableSynthVoice::renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
<<<<<<< HEAD
    jassert(isPrepared);
    
    // if voice is not active, return
    if (!isVoiceActive())
    {
        return;
    }
        
    // set size of synthBuffer audio block
    synthBuffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);

    // apply filter envelope adsr to output buffer 
    filterAdsr.applyEnvelopeToBuffer(outputBuffer, 0, numSamples);
    
    // clear synth buffer
    synthBuffer.clear();

    // create new audio block variable initilialized with synthBuffer
    juce::dsp::AudioBlock<float> audioBlock{ synthBuffer };

    //===================================
    // CALCULATING OUT OSCILLATOR

    // iterate through the necessary number of samples (from startSample up to startSample + numSamples)
    for (int sampleIndex = startSample; sampleIndex < (startSample + numSamples); sampleIndex++)
=======
    if (playing) // check to see if this voice should be playing
>>>>>>> parent of 7c7f331 (issues with filter on synth voices)
    {

        // creating a float taken from index 0 of the wtOscillators array
        auto* oscillatorSlotOne = wtOscillatorOne.getUnchecked(0);
        auto* oscillatorSlotTwo = wtOscillatorTwo.getUnchecked(0);
        auto* oscillatorSlotThree = wtOscillatorThree.getUnchecked(0);
        auto* oscillatorSlotFour = wtOscillatorFour.getUnchecked(0);
        auto* oscillatorSlotFive = wtOscillatorFive.getUnchecked(0);

        // getting the current sample from the oscillator slots and storing in sample variable
        auto slotOneSample = oscillatorSlotOne->getNextSample();
        auto slotTwoSample = oscillatorSlotTwo->getNextSample();
        auto slotThreeSample = oscillatorSlotThree->getNextSample();
        auto slotFourSample = oscillatorSlotFour->getNextSample();
        auto slotFiveSample = oscillatorSlotFive->getNextSample();

        // if, else if statement used for finding which two slots its currently between
        // and then mixes between the two
        if (wavescanBal <= 1.0)
        {
            currentSample = ((slotOneSample * (1 - wavescanBal)) + (slotTwoSample * wavescanBal)) * gainVolume;
        }
        else if (wavescanBal <= 2.0)
        {
            float normalizedWavescanVal = wavescanBal - 1.0f;
            currentSample = ((slotTwoSample * (1 - normalizedWavescanVal)) + (slotThreeSample * normalizedWavescanVal)) * gainVolume;
        }
        else if (wavescanBal <= 3.0)
        {
<<<<<<< HEAD
            float normalizedWavescanVal = wavescanBal - 2.0f;
            currentSample = ((slotThreeSample * (1 - normalizedWavescanVal)) + (slotFourSample * normalizedWavescanVal)) * gainVolume;
        }
        else if (wavescanBal <= 4.0)
        {
            float normalizedWavescanVal = wavescanBal - 3.0f;
            currentSample = ((slotFourSample * (1 - normalizedWavescanVal)) + (slotFiveSample * normalizedWavescanVal)) * gainVolume;
        }

        float fundamentalSample = fundamentalOsc.process();

        // for each channel, write the currentSample float to the output
        for (int chan = 0; chan < outputBuffer.getNumChannels(); chan++)
        {
            // The output sample is scaled by 0.1 so that it is not too loud by default
            audioBlock.addSample(chan, sampleIndex, ((currentSample * wavetableVolume) + (fundamentalSample * sineVolume)) * 0.1);
        }

    }


    //===================================

    // apply adsr envelope to buffer
    adsr.applyEnvelopeToBuffer(synthBuffer, 0, synthBuffer.getNumSamples());

    //
    //filter.process(synthBuffer);

    //gain.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));

    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
    {
        outputBuffer.addFrom(channel, startSample, synthBuffer, channel, 0, numSamples);
=======
            float envVal = env.getNextSample();



            // creating a float taken from index 0 of the wtOscillators array
            auto* oscillatorSlotOne = wtOscillatorOne.getUnchecked(0);
            auto* oscillatorSlotTwo = wtOscillatorTwo.getUnchecked(0);
            auto* oscillatorSlotThree = wtOscillatorThree.getUnchecked(0);
            auto* oscillatorSlotFour = wtOscillatorFour.getUnchecked(0);
            auto* oscillatorSlotFive = wtOscillatorFive.getUnchecked(0);

            // getting the current sample from the oscillator slots and storing in sample variable
            auto slotOneSample = oscillatorSlotOne->getNextSample();
            auto slotTwoSample = oscillatorSlotTwo->getNextSample();
            auto slotThreeSample = oscillatorSlotThree->getNextSample();
            auto slotFourSample = oscillatorSlotFour->getNextSample();
            auto slotFiveSample = oscillatorSlotFive->getNextSample();

            // if, else if statement used for finding which two slots its currently between
            // and then mixes between the two
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

            float fundamentalSample = fundamentalOsc.process() * envVal;

            // for each channel, write the currentSample float to the output
            for (int chan = 0; chan < outputBuffer.getNumChannels(); chan++)
            {
                // The output sample is scaled by 0.1 so that it is not too loud by default
                outputBuffer.addSample(chan, sampleIndex, ((currentSample * wavetableVolume) + (fundamentalSample * sineVolume)) * 0.1);
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

        float currentFilterCutoff = filterCutoff * pow(10.0f, filterEnvVal * filterEnvAmp);

        if (currentFilterCutoff > 20000.0f)
        {
            currentFilterCutoff = 20000.0f;
        }
        else if (currentFilterCutoff < 100.0f)
        {
            currentFilterCutoff = 100.0f;
        }

        synthBuffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);
        filterEnv.applyEnvelopeToBuffer(outputBuffer, 0, numSamples);        

        juce::dsp::AudioBlock<float> audioBlock{ synthBuffer };
        ladderFilter.process(synthBuffer);

        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            outputBuffer.addFrom(channel, startSample, synthBuffer, channel, 0, numSamples);
        }
>>>>>>> parent of 7c7f331 (issues with filter on synth voices)

        if (!adsr.isActive())
            clearCurrentNote();
    }

}

void WavetableSynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = outputChannels;
<<<<<<< HEAD
    
    fundamentalOsc.setSampleRate(getSampleRate());
    filterAdsr.setSampleRate(sampleRate);
    filter.prepareToPlay(sampleRate, samplesPerBlock, outputChannels);
    adsr.setSampleRate(sampleRate);

    gain.prepare(spec);
    gain.setGainLinear(0.6f);

    isPrepared = true;
=======
    ladderFilter.prepare(spec);
    ladderFilter.reset();
>>>>>>> parent of 7c7f331 (issues with filter on synth voices)
}

//===========================================================================
// SIMPLE PARAMETER SETTERS

void WavetableSynthVoice::setWavescanVal(std::atomic<float>* _wavescanBal)
{
    wavescanBal = *_wavescanBal;
}

void WavetableSynthVoice::setWavetableVolume(std::atomic<float>* _wavetableVolume)
{
    wavetableVolume = *_wavetableVolume;
}

void WavetableSynthVoice::setSineVolume(std::atomic<float>* _sineVolume)
{
    sineVolume = *_sineVolume;
}

<<<<<<< HEAD
=======
void WavetableSynthVoice::setAttack(std::atomic<float>* attack)
{
    envParams.attack = *attack;
    env.setParameters(envParams);
}

void WavetableSynthVoice::setDecay(std::atomic<float>* decay)
{
    envParams.decay = *decay;
    env.setParameters(envParams);
}

void WavetableSynthVoice::setSustain(std::atomic<float>* sustain)
{
    envParams.sustain = *sustain;
    env.setParameters(envParams);
}

void WavetableSynthVoice::setRelease(std::atomic<float>* release)
{
    envParams.release = *release;
    env.setParameters(envParams);
}

//=================================================================================

void WavetableSynthVoice::setFilterCutoff(std::atomic<float>* _filterCutoff)
{
    filterCutoff = *_filterCutoff;
}

void WavetableSynthVoice::setFilterResonance(std::atomic<float>* _filterResonance)
{
    filterResonance = *_filterResonance;
}

void WavetableSynthVoice::setFilterAttack(std::atomic<float>* filterAttack)
{
    filterEnvParams.release = *filterAttack;
    filterEnv.setParameters(filterEnvParams);
}

void WavetableSynthVoice::setFilterDecay(std::atomic<float>* filterDecay)
{
    filterEnvParams.decay = *filterDecay;
    filterEnv.setParameters(filterEnvParams);
}

void WavetableSynthVoice::setFilterSustain(std::atomic<float>* filterSustain)
{
    filterEnvParams.sustain = *filterSustain;
    filterEnv.setParameters(filterEnvParams);
}

void WavetableSynthVoice::setFilterRelease(std::atomic<float>* filterRelease)
{
    filterEnvParams.release = *filterRelease;
    filterEnv.setParameters(filterEnvParams);
}

void WavetableSynthVoice::setFilterEnvAmp(std::atomic<float>* _filterEnvAmp)
{
    filterEnvAmp = *_filterEnvAmp;

}
>>>>>>> parent of 7c7f331 (issues with filter on synth voices)

//=================================================================================

void WavetableSynthVoice::setSamplesPerBlock(int _sampsPerBlock)
{
    sampsPerBlock = _sampsPerBlock;
}


void WavetableSynthVoice::updateWavetable(int index, int slotNumber)
{
        
    // get the data name using this index
    const char* namedResource = BinaryData::namedResourceList[index];

    // create data size variable
    int dataSize;

    // using the get named resource function to set the dataSize variable to the size of the data in bytes
    const char* data = BinaryData::getNamedResource(namedResource, dataSize);

    // use these new data and data size variable to change the wavtable of the specified slot
    slots[slotNumber]->setWavetable(data, dataSize);
    
}

void WavetableSynthVoice::updateFilter(const float frequency, const float resonance)
{
    auto modulator = filterAdsr.getNextSample();
    filter.updateParameters(modulator, frequency, resonance);
}

