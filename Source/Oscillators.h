// Occillators.h

#pragma once

#define Oscillators_h

#include <cmath>

//PARENT PHASOR CLASS
class Phasor
{
public:

	// Our parent oscillator class does the key things required for most oscillators:
	// -- Handles phase
	// -- Handles setters and getters for frequency and sample rate

	// update the phase and output the next sample from the oscillator
	float process()
	{
		phase += phaseDelta;

		if (phase > 1.0f)
			phase -= 1.0f;

		return output(phase);
	}

	virtual float output(float p)
	{
		return p;
	}

	void setSampleRate(float SR)
	{
		sampleRate = SR;
	}

	void setFrequency(float freq)
	{
		frequency = freq;
		phaseDelta = frequency / sampleRate;
	}

	void setPhase(float p)
	{
		phase = p;
	}

private:
	float frequency;
	float sampleRate;
	float phase = 0.0f;
	float phaseDelta;
};

//=======================================

//CHILD class
class TriOsc : public Phasor
{
	float output(float p) override
	{
		return fabsf(p - 0.5f) - 0.5f;
	}
};

//CHILD class
class SinOsc : public Phasor
{
	float output(float p) override
	{
		return std::sin(p * 2.0 * 3.14159);
	}
};

//CHILD class
class SquareOsc : public Phasor
{
	float output(float p) override
	{
		float outVal = 0.5;
		if (p > pulseWidth)
			outVal = -0.5;
		return outVal;
	}

	void setPulseWidth(float pw)
	{
		pulseWidth = pw;
	}

private:
	float pulseWidth = 0.5f;
};