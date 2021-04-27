/*
  ==============================================================================

    WavetableSynthesiser.h
    Part of WavemorpherSynthesizer project

    Created: 17th March 2021
    Author:  Cameron Smith, UoE s1338237

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

    @param wavetable oscillator volume level
    */
    void setWavetableVolume(std::atomic<float>* _wavetableVolume);

    /**
    Sets the volume of the sinusoidal fundamental oscillator

    @param fundamental sinusoidal oscillator volume level
    */
    void setSineVolume(std::atomic<float>* _sineVolume);

    /**
    Update the parameters of the ADSR envelope

    @param attack time in seconds
    @param decay time in seconds
    @param sustain value between 0 and 1
    @param release time in seconds
    */
    void updateADSR(std::atomic<float>* attack, std::atomic<float>* decay, std::atomic<float>* sustain, std::atomic<float>* release);

    /**
    update the current filter parameters before modulation by the ADSR envelope

    @param cutoff frequency in Hz
    @param resonance value, between 0 and 1
    */
    void updateFilter(float cutoff, float resonance); 

    /**
    Update the parameters of the filter ADSR envelope

    @param attack time in seconds
    @param decay time in seconds
    @param sustain value between 0 and 1
    @param release time in seconds
    */
    void updateFilterEnv(std::atomic<float>* filterAttack, std::atomic<float>* filterDecay, std::atomic<float>* filterSustain, std::atomic<float>* filterRelease);

    /**
    Update the values of the amplitude for the filter ADSR envelope

    @param filter cutoff frequency envelope amplitude, +ve or -ve
    @param filter resonance envelope amplitude, +ve or -ve
    */
    void updateFilterEnvAmp(std::atomic<float>* _filterCutoffAmp, std::atomic<float>* _filterResonanceAmp);

    /**
    Update the parameters of the low frequency oscillator that modulates wavescanning value

    @param frequency of the LFO in Hz
    @param amplitude of the oscillation
    @param integer for determining which LFO is currently in use
    */
    void updateLfo(std::atomic<float>* _lfoFreq, std::atomic<float>* _lfoAmp, std::atomic<float>* _lfoShape);

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

    /// Simple sinusoidal oscillator for playing the fundamental frequency
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

    /// Moog style ladder filter from the JUCE DSP module
    juce::dsp::LadderFilter<float> ladderFilter;

    /// ADSR envelope for modulating the filter parameters
    juce::ADSR filterEnv;

    /// For storing the parameters of the filter ADSR envelope
    juce::ADSR::Parameters filterEnvParams;

    /// Audio buffer used to store voice samples seperate from output buffer so filtering can be applied
    juce::AudioBuffer<float> voiceBuffer;

    //===========================
    // some variables used for the filter which require global scope 

    /// variable for storing cutoff frequency
    float cutoff = 10000.0f;
    /// variable for storing cutoff frequency plus envelope amount
    float currentCutOff = 10000.0f;
    /// variable for storing resonance
    float resonance = 0.1f;
    /// variable for storing resonance plus envelope amount
    float currentResonance = 0.1f;

    /// current value of filter ADSR envelope
    float filterEnvVal = 0.0f;
    /// amplitude of effect the filter ADSR envelope has on the cutoff frequency value 
    float filterCutoffAmp = 0.0f;
    /// amplitude of effect the filter ADSR envelope has on the resonance value 
    float filterResonanceAmp = 0.0f;
    //===========================

    /// LFO Shape 1 - Sinusoidal
    SinOsc lfo1;

    /// LFO Shape 2 - Triangle
    TriOsc lfo2;
    
    /// LFO Shape 3 - Saw
    Phasor lfo3;
    
    /// LFO Shape 4 - Square
    SquareOsc lfo4;

    //===========================
    // some variables used for the LFO which require global scope 

    /// integer used to determine which lfo shape is currently selected
    int lfoShape = 1;
    /// current sample acquired from the LFO
    float lfoSample = 0.0f;
    /// current amplitude of the LFO
    float lfoAmp = 0.0f;
    //===========================
};
