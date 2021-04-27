/*
  ==============================================================================

    WavescanningSlot.h
    Part of WavemorpherSynthesizer project

    Created: 9th April 2021
    Author:  Cameron Smith, UoE s1338237

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <BinaryData.h>

class WavescanningSlot
{
public:
    WavescanningSlot(double sampleRate);


    //--------------------------------------------------------------------------
    /**
     Create wavetable from binary data and storing as a juce audio buffer "named wtFileBuffer"

    Subsequently calls the antialiasing function below to deal with the artifacting issues
     */
    void setWavetable(const void* _data, size_t _dataSize);

    //--------------------------------------------------------------------------
    /**
     Get the juce audio buffer of the wavetable at the chosen octave

     @param current octave for which the wavetable is to be used
    
     */
    juce::AudioBuffer<float> getAntialiasedWavetable(int octaveNumber);


private:

    /// For keeping list of avaible audio formats
    juce::AudioFormatManager wtFormatManager;

    /// A multi channel buffer containing floating point audio samples - the raw wavetable itself!
    juce::AudioBuffer<float> wtFileBuffer;

    /// Creating a structure for storing the antialiased wavetables
    struct wavetableOctaves {
        int wavetableLength;
        juce::AudioBuffer<float> antialiasedWavetable;
        juce::IIRFilter wtFilter;   // filter for reducing aliasing of wavetables
    };
    static constexpr int numWavetableOctaves = 10;
    wavetableOctaves mWavescanner[numWavetableOctaves];

    /// Storing sample rate
    double SR;

};