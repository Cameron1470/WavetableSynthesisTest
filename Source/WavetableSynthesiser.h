/*
  ==============================================================================

    WavetableSynthesiser.h
    Created: 7 Mar 2020 4:27:57pm
    Author:  Cameron Smith

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "WavetableOscillator.h"
#include <BinaryData.h>


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
    //--------------------------------------------------------------------------
    /**
     Create wavetable from binary data and storing as a juce audio buffer "named wtFileBuffer"

    Subsequently calls the antialiasing function below to deal with the artifacting issues
    @param the binary data wavetable file name (ie. BinaryData::FileName__wav)
    @param the binary data wavetable file size (ie. BinaryData::FileName__wavSize
     */
    void setWavetableOne(const void* inputData, size_t inputSize);



private:
    //--------------------------------------------------------------------------
    /// Should the voice be playing?
    bool playing = false;

    /// Is the voice in the process of ending?
    bool ending = false;

    /// For keeping list of avaible audio formats
    juce::AudioFormatManager wtFormatManager;

    /// A multi channel buffer containing floating point audio samples - the raw wavetable itself!
    juce::AudioBuffer<float> wtFileBuffer;

    /// Creating a structure for storing the antialiased wavetables
    struct wavescannerSlot1 {
        int wavetableLength1;
        juce::AudioBuffer<float> wavetableAntialiased1; 
        juce::IIRFilter wtFilter1;   // filter for reducing aliasing of wavetables
    };
    struct wavescannerSlot2 {
        int wavetableLength2;
        juce::AudioBuffer<float> wavetableAntialiased2;
        juce::IIRFilter wtFilter2;   // filter for reducing aliasing of wavetables
    };
    static constexpr int numWavetableOctaves = 10;
    
    wavescannerSlot1 mWavescanOne[numWavetableOctaves];
    wavescannerSlot2 mWavescanTwo[numWavetableOctaves];
    
    // number of wavescanning slots
    int wavescanningSlots = 2;

    const void** data = new const void* [wavescanningSlots];
    size_t* dataSize = new size_t[wavescanningSlots];
    


    
    


    /// an array of WavetableOscillators
    juce::OwnedArray<WavetableOscillator> wtOscillators;

    /// the ADSR envelope
    juce::ADSR env;

    /// gain used in process block
    float gain = 0.2f;

    

    //const void* wavetableOneData = BinaryData::ESW_Analog__Moog_Square_01__wav;
    //size_t wavetableOneSize = BinaryData::ESW_Analog__Moog_Square_01__wavSize;
    
};
