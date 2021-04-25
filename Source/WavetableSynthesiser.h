/*
  ==============================================================================

    WavetableSynthesiser.h

    Created: 17th March 2021
    Author:  Cameron Smith

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "WavetableOscillator.h"
#include <BinaryData.h>
#include "WavescanningSlot.h"
#include "PluginProcessor.h"
#include "Oscillators.h"
#include "LadderFilterData.h"
#include "AdsrData.h"
#include "OscData.h"



// ===========================
// ===========================
// SOUND
class WavetableSynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote      (int) override      { return true; }
    //--------------------------------------------------------------------------
    bool appliesToChannel   (int) override      { return true; }
};




// =================================
// =================================
// Synthesiser Voice - your synth code goes in here

/*!
 @class YourSynthVoice
 @abstract struct defining the DSP associated with a specific voice.
 @discussion multiple YourSynthVoice objects will be created by the Synthesiser so that it can be played polyphicially
 
 @namespace none
 @updated 2019-06-18
 */
class WavetableSynthVoice : public juce::SynthesiserVoice
{
public:
    WavetableSynthVoice();
 
    //--------------------------------------------------------------------------
    /**
    What should be done when a note starts

    @param midiNoteNumber
    @param velocity
    @param SynthesiserSound unused variable
    @param / unused variable
    */
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override;
    
    //--------------------------------------------------------------------------
    /// Called when a MIDI noteOff message is received
    /**
     What should be done when a note stops

     @param / unused variable
     @param allowTailOff bool to decie if the should be any volume decay
     */
    void stopNote(float /*velocity*/, bool allowTailOff) override;

    
    //--------------------------------------------------------------------------
    /**
     The Main DSP Block: Put your DSP code in here
     
     If the sound that the voice is playing finishes during the course of this rendered block, it must call clearCurrentNote(), to tell the synthesiser that it has finished

     @param outputBuffer pointer to output
     @param startSample position of first sample in buffer
     @param numSamples number of smaples in output buffer
     */
    void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override;
    
    //--------------------------------------------------------------------------
    void pitchWheelMoved(int) override {}
    //--------------------------------------------------------------------------
    void controllerMoved(int, int) override {}
    //--------------------------------------------------------------------------

    void prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels);

    /**
     Can this voice play a sound. I wouldn't worry about this for the time being

     @param sound a juce::SynthesiserSound* base class pointer
     @return sound cast as a pointer to an instance of YourSynthSound
     */
    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<WavetableSynthSound*> (sound) != nullptr;
    }

    /**
     Modify the mix between the wavetables

     @param wavescan balance value
     */
    void setWavescanVal(std::atomic<float>* _wavescanBal);

    /**
    Sets the volume of the wavetable oscillator

    @param wavetable volume level
    */
    void setWavetableVolume(std::atomic<float>* _wavetableVolume);

    /**
    Sets the volume of the wavetable oscillator

    @param wavetable volume level
    */
    void setSineVolume(std::atomic<float>* _sineVolume);


    /**
     Change the wavetable stored in a specified slot of the wavescanner

     @param index number used to obtain wavetable from binary data
     @param slot number of the wavescanner to be changed
     */
    void updateWavetable(int index, int slotNumber);


    void updateFilter(const float frequency, const float resonance);

    OscData& getOscillator() { return osc; }
    AdsrData& getAdsr() { return adsr; }
    AdsrData& getFilterAdsr() { return filterAdsr; }
    LadderFilterData& getFilter() { return filter; }


private:
    
    juce::AudioBuffer<float> synthBuffer;

    //==========================================================================
    
    /// Number of wavescanning slots
    const int wavescanningSlotsNumber = 5;

    /// First slot of the wavescanner, instance of the WavescanningSlot class
    WavescanningSlot slotOne;

    /// Second slot of the wavescanner, instance of the WavescanningSlot class
    WavescanningSlot slotTwo;

    /// Third slot of the wavescanner, instance of the WavescanningSlot class
    WavescanningSlot slotThree;

    /// Fourth slot of the wavescanner, instance of the WavescanningSlot class
    WavescanningSlot slotFour;

    /// Fifth slot of the wavescanner, instance of the WavescanningSlot class
    WavescanningSlot slotFive;
    
    /// Pointer array to the five slots of the wavescanner 
    WavescanningSlot* slots[5] = { &slotOne, &slotTwo, &slotThree, &slotFour, &slotFive };

    /// Owned arrays of WavetableOscillators
    juce::OwnedArray<WavetableOscillator> wtOscillatorOne;
    juce::OwnedArray<WavetableOscillator> wtOscillatorTwo;
    juce::OwnedArray<WavetableOscillator> wtOscillatorThree;
    juce::OwnedArray<WavetableOscillator> wtOscillatorFour;
    juce::OwnedArray<WavetableOscillator> wtOscillatorFive;

    //==========================================================================

    SinOsc fundamentalOsc;

    //==========================================================================

    /// Gain used in process block to reduce volume
    float gainVolume = 0.2f;

    /// Wavescan balance value, updated from the atomic float
    float wavescanBal = 2.0f;

    /// Wavetable volume level, update from the atomic float
    float wavetableVolume = 1.0f;

    /// Sine oscillator volume level, update from the atomic float
    float sineVolume = 1.0f;

    /// Current sample variable used in the process bloack
    float currentSample = 0.0f;
    
    //==========================================================================

    OscData osc;

    /// The ADSR envelope
    AdsrData adsr;

    /// The filter
    LadderFilterData filter;

    /// The filter ADSR
    AdsrData filterAdsr;

    juce::dsp::Gain<float> gain;

    bool isPrepared{ false };



};
