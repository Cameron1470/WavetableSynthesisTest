/*
  ==============================================================================

    WavescanningSlot.h
    Created: 9 Apr 2021 9:27:34am
    Author:  csmit

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <BinaryData.h>

class WavescanningSlot
{
public:
    WavescanningSlot(const void* initialData, size_t initialDataSize, double sampleRate);


    //--------------------------------------------------------------------------
    /**
     Create wavetable from binary data and storing as a juce audio buffer "named wtFileBuffer"

    Subsequently calls the antialiasing function below to deal with the artifacting issues
     */
    void setWavetable();

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

    /// Storing the name of the wavetable data in BinaryData
    const void* data;

    /// Storing the name of the wavetable data size in BinaryData
    size_t dataSize;

    /// Storing sample rate
    double SR;

};