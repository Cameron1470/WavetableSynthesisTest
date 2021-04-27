/*
  ==============================================================================

    WavetableOscillator.h

    Created: 17th March 2021
    Author:  Cameron Smtih

    Description: Class for generating the tone from the wavetable file it is 
    handed at the specifed frequency. Uses cubic spline interpolation to smooth 
    output. Quite short so I didn't feel a seperate .cpp file was necessary.
    Some aspects from: https://docs.juce.com/master/tutorial_wavetable_synth.html

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/*!
 @class WavetableOscillator
 @abstract Class for playing sound from juce::AudioBuffer
 @discussion input wavetable is played at a set frequency

 @namespace none
 */
class WavetableOscillator
{
public:
    //--------------------------------------------------------------------------
    /**
    Initialization

    @param wavetable that is to be played
    */
    WavetableOscillator(juce::AudioBuffer<float> wavetableToUse)
        : wavetable(wavetableToUse),
        tableSize(wavetable.getNumSamples())
    {
        jassert(wavetable.getNumChannels() == 1);
    }

    //--------------------------------------------------------------------------
    /**
    Set the frequency of playback

    @param frequency, how many time the juce::AudioBuffer will be played per second
    @param sample rate
    */
    void setFrequency(float frequency, float sampleRate)
    {
        auto tableSizeOverSampleRate = (float)tableSize / sampleRate;
        tableDelta = frequency * tableSizeOverSampleRate;
    }

    //--------------------------------------------------------------------------
    /**
    Get next sample

    used in synthesiser voice to retrieve next sample and incrmements the current sample
    */
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

    //--------------------------------------------------------------------------
    /**
    Get cubic spline interpolated output

    used in getNextSample to improve upon linear interpolation and achieve a smoother output

    Translated across from a function on Matthew's github
    https://github.com/mhamilt/AudioEffectsSuite/blob/bfa9a94f9bb57817b77ce8360e5afdb8e92bb076/DelayEffects/SimpleDelay.cpp#L97-L106

    @param current index in the wavetable
    */
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

private:
    /// The stored wavetable
    juce::AudioBuffer<float> wavetable;
    
    /// The size of the wavetable in samples
    const int tableSize;
    
    // Current index and table (/phase) delta
    float currentIndex = 0.0f, tableDelta = 0.0f;
};