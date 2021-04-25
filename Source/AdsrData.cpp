/*
  ==============================================================================

    AdsrData.cpp
    Created: 25 Apr 2021 3:41:57pm
    Author:  csmit

  ==============================================================================
*/

#include "AdsrData.h"

void AdsrData::update(const float attack, const float decay, const float sustain, const float release)
{
    adsrParams.attack = attack;
    adsrParams.decay = decay;
    adsrParams.sustain = sustain;
    adsrParams.release = release;

    setParameters(adsrParams);
}