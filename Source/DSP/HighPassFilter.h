#pragma once
#include <JuceHeader.h>

class HighPassFilter
{
public:
    // On définit une énumération claire pour les pentes
    enum class Slope
    {
        db6,   // 6 dB/octave (Ordre 1)
        db12,  // 12 dB/octave (Ordre 2)
        db24   // 24 dB/octave (Ordre 4, soit deux filtres d'ordre 2 en cascade)
    };

    // Le constructeur prend désormais la pente en paramètre !
    HighPassFilter(Slope chosenSlope) : filterSlope(chosenSlope)
    {
    }

    ~HighPassFilter() = default;

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;

        // On prépare les deux étages de filtrage
        filterStage1.prepare(spec);
        filterStage2.prepare(spec);

        updateCoefficients(lastCutoffFreq);
    }

    void updateParameters(float cutoffFreq)
    {
        if (cutoffFreq != lastCutoffFreq)
        {
            lastCutoffFreq = juce::jlimit(20.0f, 20000.0f, cutoffFreq);
            updateCoefficients(lastCutoffFreq);
        }
    }

    void process(const juce::dsp::ProcessContextReplacing<float>& context)
    {
        if (filterSlope == Slope::db6 || filterSlope == Slope::db12)
        {
            filterStage1.process(context);
        }
        else if (filterSlope == Slope::db24)
        {
            // Pour du 24 dB/octave, on passe deux fois dans un filtre de 12 dB/octave
            filterStage1.process(context);
            filterStage2.process(context);
        }
    }

private:
    void updateCoefficients(float cutoff)
    {
        if (filterSlope == Slope::db6)
        {
            // Ordre 1 classique (6 dB/octave)
            auto coefficients = juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(sampleRate, cutoff);
            *filterStage1.state = *coefficients;
        }
        else if (filterSlope == Slope::db12)
        {
            // Ordre 2 Butterworth standard (12 dB/octave)
            auto coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, cutoff, 0.7071f);
            *filterStage1.state = *coefficients;
        }
        else if (filterSlope == Slope::db24)
        {
            // Ordre 4 Butterworth (24 dB/octave) 
            // On cascade deux filtres d'ordre 2 avec des Q spécifiques pour garder une réponse plate (Linkwitz-Riley / Butterworth)
            auto coefficients1 = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, cutoff, 0.5412f);
            auto coefficients2 = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, cutoff, 1.3065f);

            *filterStage1.state = *coefficients1;
            *filterStage2.state = *coefficients2;
        }
    }

    // Deux processeurs stéréo en cascade
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filterStage1;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filterStage2;

    double sampleRate = 44100.0;
    float lastCutoffFreq = 20.0f;
    const Slope filterSlope;
};