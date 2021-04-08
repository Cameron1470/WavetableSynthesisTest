/*
  ==============================================================================

    WavetableSynthesiser.cpp
    Created: 6 Apr 2021 1:49:34pm
    Author:  csmit

  ==============================================================================
*/

#include "WavetableSynthesiser.h"
#include <JuceHeader.h>

WavetableSynthVoice::WavetableSynthVoice()
{
    // before processing starts, the wavetable is created through the function (defined under process block)
    createWavetable();

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

    wtOscillators.clear();

    // creating a new WavetableOscillator class from the the AudioSampleBuffer
    auto* oscillator = new WavetableOscillator(mWavetables[currentWavetable].wavetableAntialiased);

    // setting the frequency and sample rate in this class instance
    oscillator->setFrequency(freq, getSampleRate());

    // adding to the WavetableOscillator array in private variables
    wtOscillators.add(oscillator);

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
            auto* oscillator = wtOscillators.getUnchecked(0);

            // creating a sample float, using getNextSample function of the WavetableOscillator class 
            auto currentSample = oscillator->getNextSample() * gain * envVal;

            // for each channel, write the currentSample float to the output
            for (int chan = 0; chan < outputBuffer.getNumChannels(); chan++)
            {
                // The output sample is scaled by 0.2 so that it is not too loud by default
                outputBuffer.addSample(chan, sampleIndex, currentSample * 0.2);
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

void WavetableSynthVoice::createWavetable()
{
    // got most of this from here: https://docs.juce.com/master/tutorial_looping_audio_sample_buffer_advanced.html

    // allows the program to use basic audio file formats (.wav)
    wtFormatManager.registerBasicFormats();

    // loading binary data
    const void* data = BinaryData::ESW_Analog__Moog_Square_01__wav;

    // loading size of binary data
    size_t sizeBytes = BinaryData::ESW_Analog__Moog_Square_01__wavSize;

    // creating instance of juce class for reading and writing .wav files
    juce::WavAudioFormat wavFormat;

    // creating juce class for accessing block of data as a stream and handing it the source data and size
    auto* inputStream = new juce::MemoryInputStream(data, sizeBytes, false);

    // creating an audio format reader from the path
    std::unique_ptr<juce::AudioFormatReader> wtReader(wavFormat.createReaderFor(inputStream, false));

    // if statement used in juce tutorial, only used when a dialogue box is opened and user clicks cancel? maybe uneccesary?
    if (wtReader.get() != nullptr)
    {
        //setting size of private variable
        wtFileBuffer.setSize((int)wtReader->numChannels, (int)wtReader->lengthInSamples);

        //using the reader to populate AudioSampleBuffer class with the wavetable
        wtReader->read(&wtFileBuffer, 0, (int)wtReader->lengthInSamples, 0, true, true);
    }

    antialising(wtFileBuffer);
}

void WavetableSynthVoice::antialising(juce::AudioBuffer<float> _wtFileBuffer)
{
    // store variable of the sample rate
    double sampleRate = getSampleRate();

    // set cutoff frequency, single value for now. will make an array
    double cutoff[numWavetableSlots] = { 16000.0, 16000.0, 16000.0, 16000.0, 10000.0, 1000.0, 50.0, 10.0, 5.0, 1.0 };

    // find the range of values for the unfiltered buffer
    juce::Range<float> preRange = _wtFileBuffer.findMinMax(0, 0, _wtFileBuffer.getNumSamples());

    // absolute max value before filtering
    float preMaxVal = abs(std::max(preRange.getStart(), preRange.getEnd()));

    // from the original buffer creating ten wavetable across the midi note range with varying levels of filtering
    for (int wtNumber = 0; wtNumber < numWavetableSlots; wtNumber++)
    {
        // initialize filter with coefficients
        mWavetables[wtNumber].wtFilter.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, cutoff[wtNumber], 1.0));

        // reset filter
        mWavetables[wtNumber].wtFilter.reset();

        // store the wavetable length
        mWavetables[wtNumber].wavetableLength = _wtFileBuffer.getNumSamples();

        // copy the wt buffer into the structure
        mWavetables[wtNumber].wavetableAntialiased = _wtFileBuffer;

        // process wavetables with the filter
        for (int channel = 0; channel < _wtFileBuffer.getNumChannels(); channel++)
        {
            mWavetables[wtNumber].wtFilter.processSamples(mWavetables[wtNumber].wavetableAntialiased.getWritePointer(channel), mWavetables[wtNumber].wavetableLength);
            

            // find the range of values for the filtered buffer
            juce::Range<float> postRange = mWavetables[wtNumber].wavetableAntialiased.findMinMax(0, 0, _wtFileBuffer.getNumSamples());

            //find the absolute max value after filtering
            float postMaxVal = abs(std::max(postRange.getStart(), postRange.getEnd()));

            // create pointer
            float* channelSamp = mWavetables[wtNumber].wavetableAntialiased.getWritePointer(channel);

            //normalizing volume across wavetable
            for (int sample = 0; sample < mWavetables[wtNumber].wavetableLength; sample++)
            {
                channelSamp[sample] *= preMaxVal / postMaxVal;
            }
        
        }
        
       
        

    }


}