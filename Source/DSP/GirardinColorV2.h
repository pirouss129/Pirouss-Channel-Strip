#pragma once
#include <JuceHeader.h>

class GirardinColorV2
{
public:
    GirardinColorV2() = default;

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;

        filterL.prepare(spec);
        filterR.prepare(spec);

        auto coeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 1500.0f, 0.5f, 1.0f);
        *filterL.state = *coeffs;
        *filterR.state = *coeffs;
    }

    // freqIndex       : 0 à 7 (40Hz à 12kHz)
    // driveIndex      : 0 à 10 (Boost EQ réel + resserrement du Q)
    // saturatePercent : 0.0f à 100.0f (Dosage de la saturation avec Autogain)
    void updateParameters(int freqIndex, int driveIndex, float saturatePercent = 0.0f)
    {
        currentDrive = driveIndex;
        satAmount = juce::jlimit(0.0f, 1.0f, saturatePercent / 100.0f);

        static const float freqs[] = { 40.0f, 80.0f, 200.0f, 700.0f, 1500.0f, 3200.0f, 6500.0f, 12000.0f };
        float selectedFreq = freqs[juce::jlimit(0, 7, freqIndex)];

        // 1. Boost EQ direct (0 à 10 dB) — Pas d'autogain ici, le Gain monte réellement
        float boostDb = static_cast<float>(driveIndex);
        float gainLinear = juce::Decibels::decibelsToGain(boostDb);

        // 2. Facteur Q dynamique (se resserre quand le boost augmente)
        float driveNorm = static_cast<float>(driveIndex) / 10.0f;
        float qFactor = 0.5f + (driveNorm * 1.3f);

        auto coeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, selectedFreq, qFactor, gainLinear);
        *filterL.state = *coeffs;
        *filterR.state = *coeffs;

        if (satAmount <= 0.001f)
        {
            satDrive = 1.0f;
            satAutoGainCompensation = 1.0f;
            return;
        }

        // 3. Traitement spécifique du grave pour le drive (40, 80, 200 Hz)
        float lowFreqDriveBoost = 1.0f;
        if (freqIndex == 0)       lowFreqDriveBoost = 2.4f; // 40 Hz
        else if (freqIndex == 1)  lowFreqDriveBoost = 2.0f; // 80 Hz
        else if (freqIndex == 2)  lowFreqDriveBoost = 1.5f; // 200 Hz

        // 4. Drive interne de la saturation
        satDrive = (1.0f + (satAmount * 3.0f)) * lowFreqDriveBoost;

        // 5. Autogain appliqué sur le Drive (compense le gain interne injecté par le tanh)
        satAutoGainCompensation = 1.0f / std::sqrt(satDrive);

        // 6. Asymétrie renforcée dans le grave
        asymOffset = (freqIndex <= 2) ? (0.10f + (satAmount * 0.15f)) : 0.10f;
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        // 1. Filtrage EQ (100% du signal traverse l'EQ)
        juce::dsp::AudioBlock<float> block(buffer);
        const int numChannels = buffer.getNumChannels();

        if (numChannels > 0) filterL.process(juce::dsp::ProcessContextReplacing<float>(block.getSingleChannelBlock(0)));
        if (numChannels > 1) filterR.process(juce::dsp::ProcessContextReplacing<float>(block.getSingleChannelBlock(1)));

        const int numSamples = buffer.getNumSamples();

        // 2. Saturation et compensation automatique du volume de saturation
        for (int ch = 0; ch < numChannels; ++ch)
        {
            float* channelData = buffer.getWritePointer(ch);

            for (int s = 0; s < numSamples; ++s)
            {
                float x = channelData[s];

                if (satAmount > 0.001f)
                {
                    // Injection du drive
                    float driven = x * satDrive;

                    // Saturation asymétrique
                    float saturated = (std::tanh(driven + asymOffset) - std::tanh(asymOffset));

                    // Autogain sur la partie saturée
                    saturated *= satAutoGainCompensation;

                    // Blend Clean / Saturé
                    x = (x * (1.0f - satAmount)) + (saturated * satAmount);
                }

                channelData[s] = x;
            }
        }
    }

private:
    double sampleRate = 44100.0;
    int currentDrive = 0;
    float satAmount = 0.0f;

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filterL;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filterR;

    float satDrive = 1.0f;
    float satAutoGainCompensation = 1.0f;
    float asymOffset = 0.10f;
};