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

private:
    //--------------------------------------------------------------------------
    /// Should the voice be playing?
    bool playing = false;

    /// Is the voice in the process of ending?
    bool ending = false;
    
    // number of wavescanning slots
    int wavescanningSlots = 5;

    //const void** data = new const void* [wavescanningSlots];
    //size_t* dataSize = new size_t[wavescanningSlots];
    
    // instance of the WavescanningSlot class
    WavescanningSlot slotOne;
    WavescanningSlot slotTwo;
    WavescanningSlot slotThree;
    WavescanningSlot slotFour;
    WavescanningSlot slotFive;

    /// an array of WavetableOscillators
    juce::OwnedArray<WavetableOscillator> wtOscillatorOne;
    juce::OwnedArray<WavetableOscillator> wtOscillatorTwo;
    juce::OwnedArray<WavetableOscillator> wtOscillatorThree;
    juce::OwnedArray<WavetableOscillator> wtOscillatorFour;
    juce::OwnedArray<WavetableOscillator> wtOscillatorFive;

    /// the ADSR envelope
    juce::ADSR env;

    /// gain used in process block
    float gain = 0.2f;

    /// value for mixing between the wavetables
    std::atomic<float>* wavescanParameter;
    float wavescanBal = 2.0f;

    float currentSample = 0.0f;
    
};
