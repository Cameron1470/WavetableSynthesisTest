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
    // setting initial wavetable for wavescanning slot 1
    data[0] = BinaryData::ESW_Analog__Moog_Square_01__wav;
    dataSize[0] = BinaryData::ESW_Analog__Moog_Square_01__wavSize;

    // setting initial wavetable for wavescanning slot 2
    data[1] = BinaryData::ESW_Analog__Moog_Triangle_wav;
    dataSize[1] = BinaryData::ESW_Analog__Moog_Triangle_wavSize;

    // before processing starts, the wavetables are created through the function (defined under process block)
    setWavetableOne(data[0], dataSize[0]);

    setWavetableTwo(data[1], dataSize[1]);

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

    wtOscillator1.clear();
    wtOscillator2.clear();

    // creating a new WavetableOscillator class from the the AudioSampleBuffer
    auto* oscillator1 = new WavetableOscillator(mWavescanOne[currentWavetable].wavetableAntialiased1);
    auto* oscillator2 = new WavetableOscillator(mWavescanTwo[currentWavetable].wavetableAntialiased2);


    // setting the frequency and sample rate in this class instance
    oscillator1->setFrequency(freq, getSampleRate());
    oscillator2->setFrequency(freq, getSampleRate());

    // adding to the WavetableOscillator array in private variables
    wtOscillator1.add(oscillator1);
    wtOscillator2.add(oscillator2);

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
            auto* oscillator1 = wtOscillator1.getUnchecked(0);
            auto* oscillator2 = wtOscillator2.getUnchecked(0);


            // creating a sample float, using getNextSample function of the WavetableOscillator class 
            auto currentSample = ((oscillator1->getNextSample() * (1 - wavescanBal)) + (oscillator2->getNextSample() * wavescanBal)) * gain * envVal;
            //auto currentSample = (oscillator1->getNextSample() + oscillator2->getNextSample()) * gain * envVal;

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

void WavetableSynthVoice::setWavetableOne(const void* inputData, size_t inputSize)
{
    // got most of this from here: https://docs.juce.com/master/tutorial_looping_audio_sample_buffer_advanced.html

    // allows the program to use basic audio file formats (.wav)
    wtFormatManager1.registerBasicFormats();

    // loading binary data
    const void* data = inputData;

    // loading size of binary data
    size_t sizeBytes = inputSize;

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
        wtFileBuffer1.setSize((int)wtReader->numChannels, (int)wtReader->lengthInSamples);

        //using the reader to populate AudioSampleBuffer class with the wavetable
        wtReader->read(&wtFileBuffer1, 0, (int)wtReader->lengthInSamples, 0, true, true);
    }

    // store variable of the sample rate
    double sampleRate = getSampleRate();

    // set cutoff frequency, single value for now. will make an array
    double cutoff[numWavetableOctaves] = { 16000.0, 16000.0, 16000.0, 16000.0, 10000.0, 1000.0, 50.0, 10.0, 8.0, 8.0 };

    // find the range of values for the unfiltered buffer
    juce::Range<float> preRange = wtFileBuffer1.findMinMax(0, 0, wtFileBuffer1.getNumSamples());

    // absolute max value before filtering
    float preMaxVal = abs(std::max(preRange.getStart(), preRange.getEnd()));

    // from the original buffer creating ten wavetable across the midi note range with varying levels of filtering
    for (int wtNumber = 0; wtNumber < numWavetableOctaves; wtNumber++)
    {
        // initialize filter with coefficients
        mWavescanOne[wtNumber].wtFilter1.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, cutoff[wtNumber], 1.0));

        // reset filter
        mWavescanOne[wtNumber].wtFilter1.reset();

        // store the wavetable length
        mWavescanOne[wtNumber].wavetableLength1 = wtFileBuffer1.getNumSamples();

        // copy the wt buffer into the structure
        mWavescanOne[wtNumber].wavetableAntialiased1 = wtFileBuffer1;

        // process wavetables with the filter
        for (int channel = 0; channel < wtFileBuffer1.getNumChannels(); channel++)
        {
            mWavescanOne[wtNumber].wtFilter1.processSamples(mWavescanOne[wtNumber].wavetableAntialiased1.getWritePointer(channel), mWavescanOne[wtNumber].wavetableLength1);


            // find the range of values for the filtered buffer
            juce::Range<float> postRange = mWavescanOne[wtNumber].wavetableAntialiased1.findMinMax(0, 0, wtFileBuffer1.getNumSamples());

            //find the absolute max value after filtering
            float postMaxVal = abs(std::max(postRange.getStart(), postRange.getEnd()));

            // create pointer
            float* channelSamp = mWavescanOne[wtNumber].wavetableAntialiased1.getWritePointer(channel);

            //normalizing volume across wavetable
            for (int sample = 0; sample < mWavescanOne[wtNumber].wavetableLength1; sample++)
            {
                channelSamp[sample] *= preMaxVal / postMaxVal;
            }

        }


    }
}

void WavetableSynthVoice::setWavetableTwo(const void* inputData, size_t inputSize)
{
    // got most of this from here: https://docs.juce.com/master/tutorial_looping_audio_sample_buffer_advanced.html

    // allows the program to use basic audio file formats (.wav)
    wtFormatManager2.registerBasicFormats();

    // loading binary data
    const void* data = inputData;

    // loading size of binary data
    size_t sizeBytes = inputSize;

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
        wtFileBuffer2.setSize((int)wtReader->numChannels, (int)wtReader->lengthInSamples);

        //using the reader to populate AudioSampleBuffer class with the wavetable
        wtReader->read(&wtFileBuffer2, 0, (int)wtReader->lengthInSamples, 0, true, true);
    }

    // store variable of the sample rate
    double sampleRate = getSampleRate();

    // set cutoff frequency, single value for now. will make an array
    double cutoff[numWavetableOctaves] = { 16000.0, 16000.0, 16000.0, 16000.0, 10000.0, 1000.0, 50.0, 10.0, 8.0, 8.0 };

    // find the range of values for the unfiltered buffer
    juce::Range<float> preRange = wtFileBuffer2.findMinMax(0, 0, wtFileBuffer2.getNumSamples());

    // absolute max value before filtering
    float preMaxVal = abs(std::max(preRange.getStart(), preRange.getEnd()));

    // from the original buffer creating ten wavetable across the midi note range with varying levels of filtering
    for (int wtNumber = 0; wtNumber < numWavetableOctaves; wtNumber++)
    {
        // initialize filter with coefficients
        mWavescanTwo[wtNumber].wtFilter2.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, cutoff[wtNumber], 1.0));

        // reset filter
        mWavescanTwo[wtNumber].wtFilter2.reset();

        // store the wavetable length
        mWavescanTwo[wtNumber].wavetableLength2 = wtFileBuffer2.getNumSamples();

        // copy the wt buffer into the structure
        mWavescanTwo[wtNumber].wavetableAntialiased2 = wtFileBuffer2;

        // process wavetables with the filter
        for (int channel = 0; channel < wtFileBuffer2.getNumChannels(); channel++)
        {
            mWavescanTwo[wtNumber].wtFilter2.processSamples(mWavescanTwo[wtNumber].wavetableAntialiased2.getWritePointer(channel), mWavescanTwo[wtNumber].wavetableLength2);


            // find the range of values for the filtered buffer
            juce::Range<float> postRange = mWavescanTwo[wtNumber].wavetableAntialiased2.findMinMax(0, 0, wtFileBuffer2.getNumSamples());

            //find the absolute max value after filtering
            float postMaxVal = abs(std::max(postRange.getStart(), postRange.getEnd()));

            // create pointer
            float* channelSamp = mWavescanTwo[wtNumber].wavetableAntialiased2.getWritePointer(channel);

            //normalizing volume across wavetable
            for (int sample = 0; sample < mWavescanTwo[wtNumber].wavetableLength2; sample++)
            {
                channelSamp[sample] *= preMaxVal / postMaxVal;
            }

        }


    }
}

void WavetableSynthVoice::setWavescanVal(float _wavescanBal)
{
    wavescanBal = _wavescanBal;
}