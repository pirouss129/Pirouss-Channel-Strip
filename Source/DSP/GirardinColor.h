#pragma once
#include <JuceHeader.h>

class GirardinColor
{
public:
    GirardinColor() = default;

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;

        filterL.prepare(spec);
        filterR.prepare(spec);

        auto coeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 1500.0f, 0.5f, 1.0f);
        *filterL.state = *coeffs;
        *filterR.state = *coeffs;
    }

    // freqIndex  : 0 à 7
    // driveIndex : 0 à 10
    // mixPercent : 0.0f à 100.0f
    void updateParameters(int freqIndex, int driveIndex, float mixPercent = 100.0f)
    {
        currentDrive = driveIndex;
        mix = juce::jlimit(0.0f, 1.0f, mixPercent / 100.0f);

        static const float freqs[] = { 40.0f, 80.0f, 200.0f, 700.0f, 1500.0f, 3200.0f, 6500.0f, 12000.0f };
        float selectedFreq = freqs[juce::jlimit(0, 7, freqIndex)];

        // 1. Boost EQ (0 à 10 dB)
        float boostDb = static_cast<float>(driveIndex);
        float gainLinear = juce::Decibels::decibelsToGain(boostDb);

        // 2. Facteur Q progressif
        float driveNorm = static_cast<float>(driveIndex) / 10.0f;
        float qFactor = 0.5f + (driveNorm * 1.3f);

        auto coeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, selectedFreq, qFactor, gainLinear);
        *filterL.state = *coeffs;
        *filterR.state = *coeffs;

        if (driveIndex == 0 || mix <= 0.001f)
            return;

        // 3. Boost de Drive spécifique pour pousser les fréquences graves (40, 80, 200 Hz)
        float lowFreqDriveBoost = 1.0f;
        if (freqIndex == 0)       lowFreqDriveBoost = 2.4f; // 40 Hz (+7.6 dB de push interne)
        else if (freqIndex == 1)  lowFreqDriveBoost = 2.0f; // 80 Hz (+6.0 dB)
        else if (freqIndex == 2)  lowFreqDriveBoost = 1.5f; // 200 Hz (+3.5 dB)

        // 4. Intensité de saturation globale avec compensation selon la bande
        satDrive = (1.0f + (driveNorm * 2.5f)) * lowFreqDriveBoost;

        // 5. Offset d'asymétrie plus marqué dans le grave pour générer des harmoniques paires (2nd order)
        asymOffset = (freqIndex <= 2) ? 0.22f : 0.10f;

        // 6. Compensation de gain de sortie pour équilibrer le boost d'EQ et le drive interne
        float compensationDb = -boostDb * 0.65f - (lowFreqDriveBoost > 1.0f ? (lowFreqDriveBoost * 2.0f) : 0.0f);
        eqOutputCompensation = juce::Decibels::decibelsToGain(compensationDb);
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        if (currentDrive == 0 || mix <= 0.001f)
            return;

        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();

        // Duplication du signal Dry pour le Blend parallèle
        juce::AudioBuffer<float> dryBuffer;
        dryBuffer.makeCopyOf(buffer);

        // 1. Filtrage EQ
        juce::dsp::AudioBlock<float> block(buffer);
        if (numChannels > 0) filterL.process(juce::dsp::ProcessContextReplacing<float>(block.getSingleChannelBlock(0)));
        if (numChannels > 1) filterR.process(juce::dsp::ProcessContextReplacing<float>(block.getSingleChannelBlock(1)));

        // 2. Saturation & Parallel Mix
        for (int ch = 0; ch < numChannels; ++ch)
        {
            float* wetData = buffer.getWritePointer(ch);
            const float* dryData = dryBuffer.getReadPointer(ch);

            for (int s = 0; s < numSamples; ++s)
            {
                float x = wetData[s];

                // Saturation asymétrique avec compensation DC intégrée
                float driven = x * satDrive;
                float saturated = (std::tanh(driven + asymOffset) - std::tanh(asymOffset));

                // Application de la compensation de gain
                float wetSample = saturated * eqOutputCompensation;

                // Mixage Dry / Wet
                wetData[s] = (dryData[s] * (1.0f - mix)) + (wetSample * mix);
            }
        }
    }

private:
    double sampleRate = 44100.0;
    int currentDrive = 0;
    float mix = 1.0f;

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filterL;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filterR;

    float satDrive = 1.0f;
    float asymOffset = 0.10f;
    float eqOutputCompensation = 1.0f;
};