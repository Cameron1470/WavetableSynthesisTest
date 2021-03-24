/*
  ==============================================================================

    WavetableSynthesiser.h
    Created: 7 Mar 2020 4:27:57pm
    Author:  Tom Mudd

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
    WavetableSynthVoice() 
    {
        // before processing starts, the wavetable is created through the function (defined under process block)
        createWavetable();

    }
    //--------------------------------------------------------------------------
    /**
     What should be done when a note starts

     @param midiNoteNumber
     @param velocity
     @param SynthesiserSound unused variable
     @param / unused variable
     */
    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        playing = true;
        float freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        

        // creating a new WavetableOscillator class from the the AudioSampleBuffer
        auto* oscillator = new WavetableOscillator(wtFileBuffer);

        // setting the frequency and sample rate in this class instance
        oscillator->setFrequency(freq, getSampleRate());

        // adding to the WavetableOscillator array in private variables
        wtOscillators.add(oscillator);

        
    }
    //--------------------------------------------------------------------------
    /// Called when a MIDI noteOff message is received
    /**
     What should be done when a note stops

     @param / unused variable
     @param allowTailOff bool to decie if the should be any volume decay
     */
    void stopNote(float /*velocity*/, bool allowTailOff) override
    {
        clearCurrentNote();
        playing = false;

        wtOscillators.clear();
    }
    
    //--------------------------------------------------------------------------
    /**
     The Main DSP Block: Put your DSP code in here
     
     If the sound that the voice is playing finishes during the course of this rendered block, it must call clearCurrentNote(), to tell the synthesiser that it has finished

     @param outputBuffer pointer to output
     @param startSample position of first sample in buffer
     @param numSamples number of smaples in output buffer
     */
    void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        if (playing) // check to see if this voice should be playing
        {
            // iterate through the necessary number of samples (from startSample up to startSample + numSamples)
            for (int sampleIndex = startSample;   sampleIndex < (startSample+numSamples);   sampleIndex++)
            {
                // creating a float taken from index 0 of the wtOscillators array
                auto* oscillator = wtOscillators.getUnchecked(0);

                // creating a sample float, using getNextSample function of the WavetableOscillator class 
                auto currentSample = oscillator->getNextSample() * gain;
                
                // for each channel, write the currentSample float to the output
                for (int chan = 0; chan<outputBuffer.getNumChannels(); chan++)
                {
                    // The output sample is scaled by 0.2 so that it is not too loud by default
                    outputBuffer.addSample (chan, sampleIndex, currentSample * 0.2);
                }
            }
        }
    }
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

    void createWavetable()
    {
        // got most of this from here: https://docs.juce.com/master/tutorial_looping_audio_sample_buffer_advanced.html
        // seems quite advanced and I don't really understand some of whats going on here

        // allows the program to use basic audio file formats (.wav)
        wtFormatManager.registerBasicFormats();

        // loading binary data
        const void* data = BinaryData::ESW_Analog__Moog_Square_01__wav;

        // loading size of binary data
        size_t sizeBytes = BinaryData::ESW_Analog__Moog_Square_01__wavSize;

        // creating instance of juce class for reading and writing .wav files
        juce::WavAudioFormat wavFormat;

        // creating juce class for accessing block of data as a stream and handing it the source data and size
        auto* inputStream = new juce::MemoryInputStream(data, sizeBytes, false);

        // creating an audio format reader from the path
        std::unique_ptr<juce::AudioFormatReader> wtReader(wavFormat.createReaderFor(inputStream, false));

        // if statement used in juce tutorial, only used when a dialogue box is opened and user clicks cancel? maybe uneccesary?
        if (wtReader.get() != nullptr)
        {
            //setting size of private variable
            wtFileBuffer.setSize((int)wtReader->numChannels, (int)wtReader->lengthInSamples);

            //using the reader to populate AudioSampleBuffer class with the wavetable
            wtReader->read(&wtFileBuffer, 0, (int)wtReader->lengthInSamples, 0, true, true);
        }

    }

private:
    //--------------------------------------------------------------------------
    // Set up any necessary variables here
    /// Should the voice be playing?
    bool playing = false;

    // for keeping list of avaible audio formats
    juce::AudioFormatManager wtFormatManager;

    // a multi channel buffer containing floating point audio samples - the wavetable itself!
    juce::AudioBuffer<float> wtFileBuffer;

    // an array of WavetableOscillators (not sure about the owned array type, from a juce tutorial)
    juce::OwnedArray<WavetableOscillator> wtOscillators;

    // gain used in process block
    float gain = 0.2f;
};
