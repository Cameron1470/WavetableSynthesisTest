/*
  ==============================================================================

    WavescanningSlot.cpp
    Created: 9 Apr 2021 9:29:54am
    Author:  csmit

  ==============================================================================
*/

#include "WavescanningSlot.h"

WavescanningSlot::WavescanningSlot(const void* initialData, size_t initialDataSize, double sampleRate)
{
    // loading binary data
    data = initialData;

    // loading size of binary data
    dataSize = initialDataSize;

    // setting sample rate for later use
    SR = sampleRate;

    // allows the program to use basic audio file formats (.wav)
    wtFormatManager.registerBasicFormats();

    // set the inital wavetable
    setWavetable();
}

void WavescanningSlot::setWavetable()
{
    // got most of this from here: https://docs.juce.com/master/tutorial_looping_audio_sample_buffer_advanced.html

    // creating instance of juce class for reading and writing .wav files
    juce::WavAudioFormat wavFormat;

    // creating juce class for accessing block of data as a stream and handing it the source data and size
    auto* inputStream = new juce::MemoryInputStream(data, dataSize, false);

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


    // set cutoff frequency, single value for now. will make an array
    double cutoff[numWavetableOctaves] = { 16000.0, 16000.0, 16000.0, 16000.0, 10000.0, 1000.0, 50.0, 10.0, 8.0, 8.0 };

    // find the range of values for the unfiltered buffer
    juce::Range<float> preRange = wtFileBuffer.findMinMax(0, 0, wtFileBuffer.getNumSamples());

    // absolute max value before filtering
    float preMaxVal = abs(std::max(preRange.getStart(), preRange.getEnd()));

    // from the original buffer creating ten wavetable across the midi note range with varying levels of filtering
    for (int wtNumber = 0; wtNumber < numWavetableOctaves; wtNumber++)
    {
        // initialize filter with coefficients
        mWavescanner[wtNumber].wtFilter.setCoefficients(juce::IIRCoefficients::makeLowPass(SR, cutoff[wtNumber], 1.0));

        // reset filter
        mWavescanner[wtNumber].wtFilter.reset();

        // store the wavetable length
        mWavescanner[wtNumber].wavetableLength = wtFileBuffer.getNumSamples();

        // copy the wt buffer into the structure
        mWavescanner[wtNumber].antialiasedWavetable = wtFileBuffer;

        // process wavetables with the filter
        for (int channel = 0; channel < wtFileBuffer.getNumChannels(); channel++)
        {
            mWavescanner[wtNumber].wtFilter.processSamples(mWavescanner[wtNumber].antialiasedWavetable.getWritePointer(channel), mWavescanner[wtNumber].wavetableLength);


            // find the range of values for the filtered buffer
            juce::Range<float> postRange = mWavescanner[wtNumber].antialiasedWavetable.findMinMax(0, 0, wtFileBuffer.getNumSamples());

            //find the absolute max value after filtering
            float postMaxVal = abs(std::max(postRange.getStart(), postRange.getEnd()));

            // create pointer
            float* channelSamp = mWavescanner[wtNumber].antialiasedWavetable.getWritePointer(channel);

            //normalizing volume across wavetable
            for (int sample = 0; sample < mWavescanner[wtNumber].wavetableLength; sample++)
            {
                channelSamp[sample] *= preMaxVal / postMaxVal;
            }

        }


    }
}

juce::AudioBuffer<float> WavescanningSlot::getAntialiasedWavetable(int octaveNumber)
{
    return mWavescanner[octaveNumber].antialiasedWavetable;
}

void WavescanningSlot::changeWavetable(const void* _data, size_t _dataSize)
{
    data = _data;
    dataSize = _dataSize;

    setWavetable();
}