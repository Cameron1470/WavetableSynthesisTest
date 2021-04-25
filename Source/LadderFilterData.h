/*
  ==============================================================================

    LadderFilterData.h
    Created: 25 Apr 2021 3:07:44pm
    Author:  csmit

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class LadderFilterData
{
public:
    void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);
    void process(juce::AudioBuffer<float>& buffer);
    void updateParameters(const float modulator, const float frequency, const float resonance);
    void reset();

private:
    juce::dsp::LadderFilter<float> filter;
    bool isPrepared{ false };
};