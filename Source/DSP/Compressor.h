#pragma once
#include <JuceHeader.h>

class AudioCompressor
{
public:
    AudioCompressor() = default;
    ~AudioCompressor() = default;

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;

        // Configuration des ballistiques d'enveloppe
        ballisticsL.prepare(spec);
        ballisticsR.prepare(spec);

        ballisticsL.setLevelCalculationType(juce::dsp::BallisticsFilterLevelCalculationType::peak);
        ballisticsR.setLevelCalculationType(juce::dsp::BallisticsFilterLevelCalculationType::peak);

        // Lissage du Makeup Gain (20 ms de fondu)
        makeupGainSmoothed.reset(spec.sampleRate, 0.02);
        makeupGainSmoothed.setCurrentAndTargetValue(1.0f);

        grLeftDb.store(0.0f);
        grRightDb.store(0.0f);
    }

    void updateParameters(int threshIdx, int ratioIdx, int attackIdx, int releaseIdx, int makeupGainIdx)
    {
        // 1. Threshold (-40 dB à 0 dBFS)
        thresholdDb = -40.0f + static_cast<float>(threshIdx);

        // 2. Ratio & Knee Dynamique (Style Distressor)
        static const float ratioGrid[] = { 2.0f, 3.0f, 4.0f, 8.0f, 12.0f, 20.0f };
        int clampedRatioIdx = std::clamp(ratioIdx, 0, 5);
        ratio = ratioGrid[clampedRatioIdx];

        // Adaptation de la largeur du Knee selon le Ratio sélectionné
        if (clampedRatioIdx <= 1)        // 2:1 et 3:1 -> Knee très doux (Opto-style)
        {
            kneeWidth = 14.0f;
        }
        else if (clampedRatioIdx <= 3)   // 4:1 et 8:1 -> Knee normal
        {
            kneeWidth = 6.0f;
        }
        else                            // 12:1 et 20:1 -> Knee hard (mais sans artefact)
        {
            kneeWidth = 2.0f;
        }

        // 3. Grille d'Attaque (ms)
        static const float attackGrid[] = { 0.02f, 0.1f, 0.8f, 10.0f, 30.0f };
        float attackMs = attackGrid[std::clamp(attackIdx, 0, 4)];

        // 4. Grille de Release (ms)
        static const float releaseGrid[] = { 50.0f, 100.0f, 300.0f, 600.0f, 1200.0f, 400.0f };
        float releaseMs = releaseGrid[std::clamp(releaseIdx, 0, 5)];

        ballisticsL.setAttackTime(attackMs);
        ballisticsL.setReleaseTime(releaseMs);
        ballisticsR.setAttackTime(attackMs);
        ballisticsR.setReleaseTime(releaseMs);

        // 5. Makeup Gain
        float makeupDb = static_cast<float>(makeupGainIdx);
        makeupGainSmoothed.setTargetValue(juce::Decibels::decibelsToGain(makeupDb));
    }

    void process(const juce::dsp::ProcessContextReplacing<float>& context)
    {
        auto& outBlock = context.getOutputBlock();
        const auto numChannels = outBlock.getNumChannels();
        const auto numSamples = outBlock.getNumSamples();

        if (numChannels == 0 || numSamples == 0)
            return;

        float maxGrL = 0.0f;
        float maxGrR = 0.0f;

        float* chL = outBlock.getChannelPointer(0);
        float* chR = (numChannels > 1) ? outBlock.getChannelPointer(1) : nullptr;

        for (size_t i = 0; i < numSamples; ++i)
        {
            // 1. Échantillons entrants
            float sampleL = chL[i];
            float sampleR = chR ? chR[i] : sampleL;

            // 2. Conversion en dBFS (avec plancher anti-log(0))
            float dbL = juce::Decibels::gainToDecibels(std::max(std::abs(sampleL), 0.00001f));
            float dbR = juce::Decibels::gainToDecibels(std::max(std::abs(sampleR), 0.00001f));

            // 3. Calcul de la réduction de gain avec Knee Dynamique
            float rawGrL = computeDynamicKneeGR(dbL);
            float rawGrR = computeDynamicKneeGR(dbR);

            // 4. Filtrage balistique (Attaque / Release)
            float smoothGrL = ballisticsL.processSample(0, rawGrL);
            float smoothGrR = ballisticsR.processSample(0, rawGrR);

            // Peak pour les vumètres IHM
            maxGrL = std::max(maxGrL, smoothGrL);
            maxGrR = std::max(maxGrR, smoothGrR);

            // 5. Conversion du GR en gain linéaire
            float gainL = juce::Decibels::decibelsToGain(-smoothGrL);
            float gainR = juce::Decibels::decibelsToGain(-smoothGrR);

            // 6. Application du gain du compresseur + Makeup Gain lissé
            float currentMakeup = makeupGainSmoothed.getNextValue();

            chL[i] = sampleL * gainL * currentMakeup;
            if (chR)
                chR[i] = sampleR * gainR * currentMakeup;
        }

        // Transmission aux vumètres UI
        grLeftDb.store(maxGrL);
        grRightDb.store(maxGrR);
    }

    // Accesseurs IHM
    float getGainReductionLeftDb() const { return grLeftDb.load(); }
    float getGainReductionRightDb() const { return grRightDb.load(); }

private:
    // Calcul du GR avec Knee variable
    float computeDynamicKneeGR(float inputDb) const
    {
        if (thresholdDb >= 0.0f)
            return 0.0f; // Threshold à 0 = Bypass / Transparent

        float delta = inputDb - thresholdDb;
        float halfKnee = kneeWidth * 0.5f;

        if (delta <= -halfKnee)
            return 0.0f; // En dessous de la zone de transition

        float slope = 1.0f - (1.0f / ratio);

        if (delta >= halfKnee)
            return delta * slope; // Au-dessus du knee : compression plein ratio

        // Dans le Knee : transition parabolique douce
        float num = delta + halfKnee;
        return slope * (num * num) / (2.0f * kneeWidth);
    }

    double sampleRate = 44100.0;
    float thresholdDb = 0.0f;
    float ratio = 2.0f;
    float kneeWidth = 6.0f; // Valeur dynamique (14 dB, 6 dB ou 2 dB)

    juce::dsp::BallisticsFilter<float> ballisticsL;
    juce::dsp::BallisticsFilter<float> ballisticsR;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> makeupGainSmoothed;

    std::atomic<float> grLeftDb{ 0.0f };
    std::atomic<float> grRightDb{ 0.0f };
};