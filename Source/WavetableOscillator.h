/*
  ==============================================================================

    WavetableOscillator.h
    Created: 17 Mar 2021 8:53:50pm
    Author:  csmit

    Largely taken from: https://docs.juce.com/master/tutorial_wavetable_synth.html

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class WavetableOscillator
{
public:
    WavetableOscillator(const juce::AudioSampleBuffer& wavetableToUse)
        : wavetable(wavetableToUse),
        tableSize(wavetable.getNumSamples())
    {
        jassert(wavetable.getNumChannels() == 1);
    }

    void setFrequency(float frequency, float sampleRate)
    {
        auto tableSizeOverSampleRate = (float)tableSize / sampleRate;
        tableDelta = frequency * tableSizeOverSampleRate;
    }

    forcedinline float getNextSample() noexcept
    {
        // get number of samples in the wavetable
        auto tableSize = (unsigned int)wavetable.getNumSamples();

        auto currentSample = getSplineOut(currentIndex);

        // if current index + delta is larger than table size. subtract table size
        if ((currentIndex += tableDelta) > (float)tableSize) 
            currentIndex -= (float)tableSize;

        // return sample
        return currentSample;
    }

    float getSplineOut(float currentIndex)
    {
        const int n0 = floor(currentIndex);
        const int n1 = (n0 + 1) % tableSize;
        const int n2 = (n0 + 2) % tableSize;
        const float alpha = currentIndex - n0;

        auto* table = wavetable.getReadPointer(0);
        const float a = table[n1];
        const float c = ((3.0f * (table[n2] - table[n1])) - (3.0f * (table[n1] - table[n0]))) * 0.25f;
        const float b = (table[n2] - table[n1]) - (2.0f * c * 0.33333f);
        const float d = (-c) * 0.33333f;
        return a + (b * alpha) + (c * alpha * alpha) + (d * alpha * alpha * alpha);
    }

    float getTableDelta()
    {
        return tableDelta;
    }

private:
    const juce::AudioSampleBuffer& wavetable;
    const int tableSize;
    float currentIndex = 0.0f, tableDelta = 0.0f;
};