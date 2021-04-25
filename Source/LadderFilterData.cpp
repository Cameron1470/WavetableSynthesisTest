/*
  ==============================================================================

    LadderFilterData.cpp
    Created: 25 Apr 2021 3:07:44pm
    Author:  csmit

  ==============================================================================
*/

#include "LadderFilterData.h"
void LadderFilterData::prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels)
{
    filter.reset();

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = numChannels;

    filter.prepare(spec);

    isPrepared = true;
}

void LadderFilterData::process(juce::AudioBuffer<float>& buffer)
{
    jassert(isPrepared);

    juce::dsp::AudioBlock<float> block{ buffer };
    filter.process(juce::dsp::ProcessContextReplacing<float> { block });
}

void LadderFilterData::updateParameters(const float modulator, const float frequency, const float resonance)
{
    float modulatedFreq = frequency * modulator;
    modulatedFreq = std::fmax(std::fmin(modulatedFreq, 20000.0f), 20.0f);

    filter.setCutoffFrequencyHz(modulatedFreq);
    filter.setResonance(resonance);
}

void LadderFilterData::reset()
{
    filter.reset();
}