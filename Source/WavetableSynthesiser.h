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

    
    void prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels);


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
     Modify the attack of the ADSR envelope

     @param attack time in seconds
     */
    void setAttack(std::atomic<float>* attack);

    /**
     Modify the decay of the ADSR envelope

     @param decay time in seconds
     */
    void setDecay(std::atomic<float>* decay);

    /**
     Modify the sustain value of the ADSR envelope

     @param sustain value between 0 and 1
     */
    void setSustain(std::atomic<float>* sustain);

    /**
     Modify the release of the ADSR envelope

     @param release time in seconds
     */
    void setRelease(std::atomic<float>* release);

    void updateFilter(float cutoff, float resonance); 

    void updateFilterEnv(std::atomic<float>* filterAttack, std::atomic<float>* filterDecay, std::atomic<float>* filterSustain, std::atomic<float>* filterRelease);

    void updateFilterEnvAmp(std::atomic<float>* _filterCutoffAmp, std::atomic<float>* _filterResonanceAmp);

    /**
     Change the wavetable stored in a specified slot of the wavescanner

     @param index number used to obtain wavetable from binary data
     @param slot number of the wavescanner to be changed
     */
    void updateWavetable(int index, int slotNumber);


private:
    //--------------------------------------------------------------------------
    /// Should the voice be playing?
    bool playing = false;

    /// Is the voice in the process of ending?
    bool ending = false;
    
    
    
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

    /// The ADSR envelope
    juce::ADSR env;

    /// Gain used in process block to reduce volume
    float gain = 0.2f;

    /// Wavescan balance value, updated from the atomic float
    float wavescanBal = 2.0f;

    /// Wavetable volume level, update from the atomic float
    float wavetableVolume = 1.0f;

    /// Sine oscillator volume level, update from the atomic float
    float sineVolume = 1.0f;

    /// Current sample variable used in the process bloack
    float currentSample = 0.0f;
    
    /// For storing the parmeters of the ADSR envelope
    juce::ADSR::Parameters envParams;

    juce::dsp::LadderFilter<float> ladderFilter;

    juce::ADSR filterEnv;

    juce::ADSR::Parameters filterEnvParams;

    float cutoff = 10000.0f;

    float currentCutOff = 10000.0f;

    float resonance = 0.1f;
    float currentResonance = 0.1f;

    float filterEnvVal;

    float filterCutoffAmp;
    float filterResonanceAmp;

    juce::AudioBuffer<float> voiceBuffer;

};
