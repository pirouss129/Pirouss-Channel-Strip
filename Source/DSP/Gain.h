#pragma once
#include <JuceHeader.h>

class Gain
{
public:
    Gain() = default;

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;

        // On initialise le lisseur de rampe de gain (temps de transition de 0.02 secondes)
        smoothedGain.reset(sampleRate, 0.02);
    }

    void updateParameters(float gainDb)
    {
        if (gainDb != lastGainDb)
        {
            lastGainDb = gainDb;
            // Conversion dB -> Linéaire
            float targetGain = juce::Decibels::decibelsToGain(gainDb);
            smoothedGain.setTargetValue(targetGain);
        }
    }

    void process(const juce::dsp::ProcessContextReplacing<float>& context)
    {
        auto& block = context.getOutputBlock();
        auto numChannels = block.getNumChannels();
        auto numSamples = block.getNumSamples();

        // Si le gain est à 0 dB et que le lisseur a fini sa course, on court-circuite pour économiser le CPU
        if (lastGainDb == 0.0f && !smoothedGain.isSmoothing())
            return;

        for (size_t sample = 0; sample < numSamples; ++sample)
        {
            // On récupère le multiplicateur de gain lissé pour cet échantillon
            float currentGain = smoothedGain.getNextValue();

            for (size_t channel = 0; channel < numChannels; ++channel)
            {
                block.getChannelPointer(channel)[sample] *= currentGain;
            }
        }
    }

private:
    double sampleRate = 44100.0;
    float lastGainDb = 0.0f;

    juce::LinearSmoothedValue<float> smoothedGain{ 1.0f }; // Par défaut gain de 1.0 (0 dB)
};