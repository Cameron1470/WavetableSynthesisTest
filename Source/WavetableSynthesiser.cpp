/*
  ==============================================================================

    WavetableSynthesiser.cpp
    Part of WavemorpherSynthesizer project

    Created: 6th April 2021
    Author:  Cameron Smith, UoE s1338237

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
    // set sample rate of ADSR envelope
    env.setSampleRate(getSampleRate());
    
}

void WavetableSynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/)
{
    // change the current playing state of the voice
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

    // set the frequency for the fundamental oscillator
    fundamentalOsc.setFrequency(freq);

    // reset and start envelope
    env.reset();
    env.noteOn();

    filterEnv.reset();
    filterEnv.noteOn();
}

void WavetableSynthVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    env.noteOff();
    filterEnv.noteOff();
    ending = true;
}

void WavetableSynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels)
{
    // reset filter before beginning
    ladderFilter.reset();

    // further setting up required for juce DSP filter
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = outputChannels;
    ladderFilter.prepare(spec);

    // setting size of voice buffer that will be used for applying filters to individual voices
    voiceBuffer.setSize(outputChannels, samplesPerBlock);

    // set sample rates for all the LFO shapes
    lfo1.setSampleRate(sampleRate);
    lfo2.setSampleRate(sampleRate); 
    lfo3.setSampleRate(sampleRate);
    lfo4.setSampleRate(sampleRate);

    // set sample rate for the fundamental oscillator 
    fundamentalOsc.setSampleRate(sampleRate);

}


void WavetableSynthVoice::renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    

    if (playing) // check to see if this voice should be playing
    {
        jassert(numSamples <= voiceBuffer.getNumSamples());
        
        // creating a proxy audio buffer to apply Juce DSP filter to before adding to output buffer
        juce::AudioBuffer<float> proxy(voiceBuffer.getArrayOfWritePointers(), voiceBuffer.getNumChannels(), startSample, numSamples);
        proxy.clear();


        // iterate through the necessary number of samples (from startSample up to startSample + numSamples)
        for (int sample = 0; sample < proxy.getNumSamples(); sample++)
        {
            // get next sample from the amplitude and filter ADSR envelopes
            float envVal = env.getNextSample();
            filterEnvVal = filterEnv.getNextSample();

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

            // switch statement for determining which lfo shape to use and get the next sample of
            switch (lfoShape) {
            case 1:
                lfoSample = lfo1.process();
                break;
            case 2:
                lfoSample = lfo2.process();
                break;
            case 3:
                lfoSample = lfo3.process();
                break;
            case 4:
                lfoSample = lfo4.process();
                break;
            }
            
            // find current wavescan parameter including modulation by lfo
            float modulatedWavescanBal = wavescanBal + (lfoSample * lfoAmp);

            // brickwalling the modulated wavescanning parameter so it doesn't exceed bounds
            if (modulatedWavescanBal < 0.0f)
                modulatedWavescanBal = 0.0f;
            else if (modulatedWavescanBal > 4.0f)
                modulatedWavescanBal = 4.0f;

            // if, else if statement used for finding which two slots its currently between
            // and then mixes between the two
            if (modulatedWavescanBal <= 1.0)
            {
                currentSample = ((slotOneSample * (1 - modulatedWavescanBal)) + (slotTwoSample * modulatedWavescanBal)) * gain * envVal;
            }
            else if (wavescanBal <= 2.0)
            {
                float normalizedWavescanVal = modulatedWavescanBal - 1.0f;
                currentSample = ((slotTwoSample * (1 - normalizedWavescanVal)) + (slotThreeSample * normalizedWavescanVal)) * gain * envVal;
            }
            else if (wavescanBal <= 3.0)
            {
                float normalizedWavescanVal = modulatedWavescanBal - 2.0f;
                currentSample = ((slotThreeSample * (1 - normalizedWavescanVal)) + (slotFourSample * normalizedWavescanVal)) * gain * envVal;
            }
            else if (wavescanBal <= 4.0)
            {
                float normalizedWavescanVal = modulatedWavescanBal - 3.0f;
                currentSample = ((slotFourSample * (1 - normalizedWavescanVal)) + (slotFiveSample * normalizedWavescanVal)) * gain * envVal;
            }

            // get next sample of the basic sine wave fundamental oscillator
            float fundamentalSample = fundamentalOsc.process() * envVal;

            // for each channel, write the currentSample float to the output
            for (int channel = 0; channel < proxy.getNumChannels(); channel++)
            {
                // The output sample is scaled by 0.1 so that it is not too loud by default
                proxy.addSample(channel, sample, ((currentSample * wavetableVolume) + (fundamentalSample * sineVolume)) * 0.5);
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
        
        // calculate cutoff frequency and resonance values with current modulation amount 
        if (filterCutoffAmp >= 0.0f)
            currentCutOff = cutoff + filterEnvVal * filterCutoffAmp* (20000.0f - cutoff);
        else if (filterCutoffAmp < 0.0f)
            currentCutOff = cutoff + filterEnvVal * filterCutoffAmp * (cutoff - 100.0f);

        if (filterResonanceAmp >= 0.0f)
            currentResonance = resonance + (filterEnvVal * filterResonanceAmp * (1.0f - resonance));
        else if (filterResonanceAmp < 0.0f)
            currentResonance = resonance + (filterEnvVal * filterResonanceAmp * resonance);

        // update the filter with the parameter values plus the envelope modulation
        ladderFilter.setCutoffFrequencyHz(currentCutOff);
        ladderFilter.setResonance(currentResonance);

        // apply this filter on the proxy sample block
        juce::dsp::AudioBlock<float> sampleBlock(proxy);
        ladderFilter.process(juce::dsp::ProcessContextReplacing<float>(sampleBlock));
        
        // add the proxy audio buffer to the output buffer
        for (int sample = 0; sample < numSamples; ++sample)
        {
            for (int channel = 0; channel < proxy.getNumChannels(); channel++)
            {
                outputBuffer.addSample(channel, startSample, proxy.getSample(channel, sample));
            }

            ++startSample;
        }
    }

   
    
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

void WavetableSynthVoice::updateADSR(std::atomic<float>* attack, std::atomic<float>* decay, std::atomic<float>* sustain, std::atomic<float>* release)
{
    envParams.attack = *attack;
    envParams.decay = *decay;
    envParams.sustain = *sustain;
    envParams.release = *release;
    env.setParameters(envParams);
}


void WavetableSynthVoice::updateFilter(float _cutoff, float _resonance)
{
    cutoff = _cutoff;
    resonance = _resonance;
}

void WavetableSynthVoice::updateFilterEnv(std::atomic<float>* filterAttack, std::atomic<float>* filterDecay, std::atomic<float>* filterSustain, std::atomic<float>* filterRelease)
{
    filterEnvParams.attack = *filterAttack;
    filterEnvParams.decay = *filterDecay;
    filterEnvParams.sustain = *filterSustain;
    filterEnvParams.release = *filterRelease;

    filterEnv.setParameters(filterEnvParams);

}

void WavetableSynthVoice::updateFilterEnvAmp(std::atomic<float>* _filterCutoffAmp, std::atomic<float>* _filterResonanceAmp)
{
    filterCutoffAmp = *_filterCutoffAmp;
    filterResonanceAmp = *_filterResonanceAmp;
}

void WavetableSynthVoice::updateLfo(std::atomic<float>* _lfoFreq, std::atomic<float>* _lfoAmp, std::atomic<float>* _lfoShape)
{
    lfoAmp = *_lfoAmp;
    lfoShape = int(*_lfoShape + 1);
    lfo1.setFrequency(*_lfoFreq);
    lfo2.setFrequency(*_lfoFreq);
    lfo3.setFrequency(*_lfoFreq);
    lfo4.setFrequency(*_lfoFreq);
}

//=================================================================================

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

