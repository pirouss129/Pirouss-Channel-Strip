#pragma once
#include <JuceHeader.h>

class PiroussExpander
{
public:
    PiroussExpander() = default;
    ~PiroussExpander() = default;

    // --- PREPARATION DU DSP ---
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        currentSampleRate = spec.sampleRate;
        reset();
    }

    void prepare(double sampleRate, int /*samplesPerBlock*/)
    {
        currentSampleRate = sampleRate;
        reset();
    }

    void reset()
    {
        envelope = 0.0f;
        gainSmooth = 1.0f;
        currentGainReductionDb = 0.0f;
    }

    // --- MISE À JOUR DES PARAMÈTRES ---
    // ratioIndex : 0 = OFF (1:1), 1 = 1:2, 2 = 1:4, 3 = 1:8, 4 = GATE
    void setParameters(float thresholdDb, int ratioIndex, float rangeDb, float attackMs, float releaseMs)
    {
        this->thresholdDb = thresholdDb; // Reçoit directement de -50.0f à 0.0f dBFS
        this->rangeDb = std::abs(rangeDb);

        // Position OFF uniquement gérée par le Ratio
        if (ratioIndex == 0)
        {
            this->ratio = 1.0f; // Pas d'expansion
        }
        else
        {
            switch (ratioIndex)
            {
            case 1:  this->ratio = 2.0f;   break; // 1:2
            case 2:  this->ratio = 4.0f;   break; // 1:4
            case 3:  this->ratio = 8.0f;   break; // 1:8
            case 4:  this->ratio = 100.0f; break; // GATE
            default: this->ratio = 1.0f;   break;
            }
        }

        // Calcul des coefficients d'enveloppe
        float attackSec = std::max(0.0001f, attackMs * 0.001f);
        float releaseSec = std::max(0.001f, releaseMs * 0.001f);

        attackCoef = std::exp(-1.0f / (attackSec * static_cast<float>(currentSampleRate)));
        releaseCoef = std::exp(-1.0f / (releaseSec * static_cast<float>(currentSampleRate)));
    }

    // --- TRAITEMENT DU BUFFER AUDIO (STEREO / MONO) ---
    void process(juce::AudioBuffer<float>& buffer)
    {
        // Bypass complet si Ratio à 1.0f (Position OFF)
        if (ratio <= 1.001f)
        {
            gainSmooth = 1.0f;
            currentGainReductionDb = 0.0f;
            return;
        }

        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();

        auto* channelL = buffer.getWritePointer(0);
        auto* channelR = (numChannels > 1) ? buffer.getWritePointer(1) : nullptr;

        float maxReductionInBlock = 0.0f;

        for (int i = 0; i < numSamples; ++i)
        {
            // 1. Détection de crête (Stereo Linked)
            float sampleL = std::abs(channelL[i]);
            float sampleR = channelR ? std::abs(channelR[i]) : 0.0f;
            float inputPeak = std::max(sampleL, sampleR);

            // 2. Détecteur d'enveloppe
            if (inputPeak > envelope)
                envelope = attackCoef * envelope + (1.0f - attackCoef) * inputPeak;
            else
                envelope = releaseCoef * envelope + (1.0f - releaseCoef) * inputPeak;

            // 3. Conversion de l'enveloppe en dB
            float envDb = juce::Decibels::gainToDecibels(envelope, -100.0f);

            // 4. Calcul du Gain Target en dB
            float targetGainDb = 0.0f;

            if (envDb < thresholdDb)
            {
                // Formule de la courbe d'expansion sous le seuil
                targetGainDb = (envDb - thresholdDb) * (ratio - 1.0f);

                // Application de la plage maximale d'atténuation (Range)
                targetGainDb = std::max(targetGainDb, -rangeDb);
            }

            // Garde en mémoire la réduction max du buffer pour le Meter (en valeur absolue positive, ex: 12 dB)
            maxReductionInBlock = std::max(maxReductionInBlock, std::abs(targetGainDb));

            // 5. Lissage du gain (conversion linéaire + filtre passe-bas basique pour éviter les pops)
            float targetGainLinear = juce::Decibels::decibelsToGain(targetGainDb);
            gainSmooth = gainSmooth * 0.9f + targetGainLinear * 0.1f;

            // 6. Application du gain sur le signal
            channelL[i] *= gainSmooth;
            if (channelR)
                channelR[i] *= gainSmooth;
        }

        // Met à jour la variable de réduction de gain lue par le PluginProcessor / Editor
        currentGainReductionDb = maxReductionInBlock;
    }

    // Renvoie la réduction de gain en dB (valeur positive, ex: 0.0f = aucun traitement, 12.0f = -12 dB d'atténuation)
    float getCurrentGainReductionDb() const
    {
        return currentGainReductionDb;
    }

private:
    double currentSampleRate = 44100.0;

    float thresholdDb = -30.0f;
    float ratio = 1.0f;
    float rangeDb = 20.0f;

    float attackCoef = 0.0f;
    float releaseCoef = 0.0f;

    float envelope = 0.0f;
    float gainSmooth = 1.0f;
    float currentGainReductionDb = 0.0f;
};