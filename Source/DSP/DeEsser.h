#pragma once
#include <JuceHeader.h>

class DeEsser
{
public:
    DeEsser() = default;
    ~DeEsser() = default;

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;

        sidechainFilterL.reset();
        sidechainFilterR.reset();
        splitFilterL.reset();
        splitFilterR.reset();

        ballisticsL.prepare(spec);
        ballisticsR.prepare(spec);

        ballisticsL.setLevelCalculationType(juce::dsp::BallisticsFilterLevelCalculationType::peak);
        ballisticsR.setLevelCalculationType(juce::dsp::BallisticsFilterLevelCalculationType::peak);

        // Attaque très rapide (1 ms), Release doux (80 ms) pour une réponse type opto
        ballisticsL.setAttackTime(1.0f);
        ballisticsL.setReleaseTime(80.0f);
        ballisticsR.setAttackTime(1.0f);
        ballisticsR.setReleaseTime(80.0f);

        grLeftDb.store(0.0f);
        grRightDb.store(0.0f);
    }

    void reset()
    {
        sidechainFilterL.reset();
        sidechainFilterR.reset();
        splitFilterL.reset();
        splitFilterR.reset();
        ballisticsL.reset();
        ballisticsR.reset();
        grLeftDb.store(0.0f);
        grRightDb.store(0.0f);
    }

    void updateParameters(int freqIdx, int threshIdx, bool isSplitMode)
    {
        splitMode = isSplitMode;

        thresholdDb = static_cast<float>(threshIdx) - 80.0f;

        // 1. Récupération de la fréquence sélectionnée
        static const float freqGrid[] = { 3000.0f, 4500.0f, 6000.0f, 7500.0f, 9000.0f, 11000.0f, 13000.0f };
        selectedFreq = freqGrid[juce::jlimit(0, 6, freqIdx)];

        // 2. Le passe-haut du sidechain s'aligne sur la fréquence choisie
        auto scCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, selectedFreq);
        sidechainFilterL.coefficients = scCoeffs;
        sidechainFilterR.coefficients = scCoeffs;
    }

    void process(const juce::dsp::ProcessContextReplacing<float>& context)
    {
        if (thresholdDb >= 0.0f)
        {
            grLeftDb.store(0.0f);
            grRightDb.store(0.0f);
            return;
        }

        auto& outBlock = context.getOutputBlock();
        const auto numChannels = outBlock.getNumChannels();
        const auto numSamples = outBlock.getNumSamples();

        if (numChannels == 0 || numSamples == 0)
            return;

        auto inputBlock = context.getInputBlock();
        scBuffer.setSize((int)numChannels, (int)numSamples, false, false, true);

        for (size_t ch = 0; ch < numChannels; ++ch)
            scBuffer.copyFrom((int)ch, 0, inputBlock.getChannelPointer(ch), (int)numSamples);

        float* scL = scBuffer.getWritePointer(0);
        float* scR = (numChannels > 1) ? scBuffer.getWritePointer(1) : scL;

        // 1. Détection passe-haut du sidechain
        for (size_t i = 0; i < numSamples; ++i)
        {
            scL[i] = sidechainFilterL.processSample(scL[i]);
            if (numChannels > 1)
                scR[i] = sidechainFilterR.processSample(scR[i]);
        }

        float* chL = outBlock.getChannelPointer(0);
        float* chR = (numChannels > 1) ? outBlock.getChannelPointer(1) : nullptr;

        float maxGrL = 0.0f;
        float maxGrR = 0.0f;

        for (size_t i = 0; i < numSamples; ++i)
        {
            float scDbL = juce::Decibels::gainToDecibels(std::max(std::abs(scL[i]), 0.00001f));
            float scDbR = juce::Decibels::gainToDecibels(std::max(std::abs(scR[i]), 0.00001f));

            float overL = std::max(0.0f, scDbL - thresholdDb);
            float overR = std::max(0.0f, scDbR - thresholdDb);

            // Ratio progressif type Soft-Knee (4:1)
            float rawGrL = overL * 0.75f;
            float rawGrR = overR * 0.75f;

            float smoothGrL = ballisticsL.processSample(0, rawGrL);
            float smoothGrR = ballisticsR.processSample(0, rawGrR);

            maxGrL = std::max(maxGrL, smoothGrL);
            maxGrR = std::max(maxGrR, smoothGrR);

            if (!splitMode)
            {
                // MODE WIDE : Réduction de gain broad-band
                float gainL = juce::Decibels::decibelsToGain(-smoothGrL);
                float gainR = juce::Decibels::decibelsToGain(-smoothGrR);

                chL[i] *= gainL;
                if (chR) chR[i] *= gainR;
            }
            else
            {
                // MODE SPLIT : High-Shelf dynamique
                float shelfGrL = std::min(smoothGrL, 18.0f); // Max -18 dB sur le shelf
                float shelfGrR = std::min(smoothGrR, 18.0f);

                auto shelfCoeffsL = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
                    sampleRate, selectedFreq, 0.707f, juce::Decibels::decibelsToGain(-shelfGrL));
                auto shelfCoeffsR = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
                    sampleRate, selectedFreq, 0.707f, juce::Decibels::decibelsToGain(-shelfGrR));

                splitFilterL.coefficients = shelfCoeffsL;
                splitFilterR.coefficients = shelfCoeffsR;

                chL[i] = splitFilterL.processSample(chL[i]);
                if (chR) chR[i] = splitFilterR.processSample(chR[i]);
            }
        }

        grLeftDb.store(maxGrL);
        grRightDb.store(maxGrR);
    }

    float getGainReductionLeftDb() const { return grLeftDb.load(); }
    float getGainReductionRightDb() const { return grRightDb.load(); }

private:
    double sampleRate = 44100.0;
    float thresholdDb = 0.0f;
    float selectedFreq = 6000.0f; // 🟢 Déclaré ici
    bool splitMode = false;

    juce::AudioBuffer<float> scBuffer;

    juce::dsp::IIR::Filter<float> sidechainFilterL;
    juce::dsp::IIR::Filter<float> sidechainFilterR;

    juce::dsp::IIR::Filter<float> splitFilterL;
    juce::dsp::IIR::Filter<float> splitFilterR;

    juce::dsp::BallisticsFilter<float> ballisticsL;
    juce::dsp::BallisticsFilter<float> ballisticsR;

    std::atomic<float> grLeftDb{ 0.0f };
    std::atomic<float> grRightDb{ 0.0f };
};