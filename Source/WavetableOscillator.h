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
#include "LagrangeTable.h"

class WavetableOscillator
{
public:
    WavetableOscillator(const juce::AudioSampleBuffer& wavetableToUse)
        : wavetable(wavetableToUse),
        tableSize(wavetable.getNumSamples()),
        lagrangeTab(4, 100)
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

        // create a temporary variable of the current index
        auto index0 = (unsigned int)currentIndex;             
        
        // now finding the next index -- if this matches the table size it will wrap back to 0
        auto index1 = (index0 + 1) % tableSize;

        // fractional different between rounded index0 and actual current index
        auto frac = currentIndex - (float)index0; 

        // create read pointer and read the values at the two index points
        auto* table = wavetable.getReadPointer(0);        
        auto value0 = table[index0];
        auto value1 = table[index1];

        // linearly interpolate
        auto currentSample = value0 + frac * (value1 - value0);

        // if current index + delta is larger than table size. subtract table size
        if ((currentIndex += tableDelta) > (float)tableSize) 
            currentIndex -= (float)tableSize;

        // return sample
        return currentSample;
    }

    

private:
    const juce::AudioSampleBuffer& wavetable;
    const int tableSize;
    float currentIndex = 0.0f, tableDelta = 0.0f;

    LagrangeTable lagrangeTab;
};