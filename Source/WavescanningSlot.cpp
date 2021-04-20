/*
  ==============================================================================

    WavescanningSlot.cpp
    Created: 9 Apr 2021 9:29:54am
    Author:  Cameron Smith

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
    // creating instance of juce class for reading and writing .wav files
    juce::WavAudioFormat wavFormat;

    // creating juce class for accessing block of data as a stream and handing it the source data and size
    auto* inputStream = new juce::MemoryInputStream(data, dataSize, false);

    // creating an audio format reader from the path
    std::unique_ptr<juce::AudioFormatReader> wtReader(wavFormat.createReaderFor(inputStream, false));

    if (wtReader.get() != nullptr)
    {
        // create temporary audio buffer
        juce::AudioBuffer<float> wtTempBuffer;
        
        //setting size of private variable
        wtTempBuffer.setSize((int)wtReader->numChannels, (int)wtReader->lengthInSamples);

        //using the reader to populate AudioSampleBuffer class with the wavetable
        wtReader->read(&wtTempBuffer, 0, (int)wtReader->lengthInSamples, 0, true, true);

        // storing in private member variable
        wtFileBuffer = wtTempBuffer;
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
    // update private variables with BinaryData information of the new wavetable
    data = _data;
    dataSize = _dataSize;

    // call the set wavetable function which will now use the new wavetable info to update the slot
    setWavetable();
}