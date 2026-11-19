#pragma once
#include <JuceHeader.h>

class Equalizer
{
public:
    Equalizer() = default;
    ~Equalizer() = default;

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;

        // Préparation des 4 processeurs stéréo
        hfBand.prepare(spec);
        mhfBand.prepare(spec);
        mlfBand.prepare(spec);
        lfBand.prepare(spec);

        // Initialisation avec des coefficients neutres
        auto neutralCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 1000.0f, 1.0f, 1.0f);
        *hfBand.state = *neutralCoeffs;
        *mhfBand.state = *neutralCoeffs;
        *mlfBand.state = *neutralCoeffs;
        *lfBand.state = *neutralCoeffs;
    }

    void updateParameters(int hfFreqIdx, int hfGainIdx, int hfQIdx,
        int mhfFreqIdx, int mhfGainIdx, int mhfQIdx,
        int mlfFreqIdx, int mlfGainIdx, int mlfQIdx,
        int lfFreqIdx, int lfGainIdx, int lfQIdx)
    {
        // 1. Grille de Gains commune étendue (15 positions)
        static const float gainGrid[] = {
            -20.0f, -15.0f, -12.0f, -9.0f, -6.0f, -4.0f, -2.0f,
             0.0f,
             2.0f, 4.0f, 6.0f, 9.0f, 12.0f, 15.0f, 20.0f
        };

        float hfGain = juce::Decibels::decibelsToGain(gainGrid[juce::jlimit(0, 14, hfGainIdx)]);
        float mhfGain = juce::Decibels::decibelsToGain(gainGrid[juce::jlimit(0, 14, mhfGainIdx)]);
        float mlfGain = juce::Decibels::decibelsToGain(gainGrid[juce::jlimit(0, 14, mlfGainIdx)]);
        float lfGain = juce::Decibels::decibelsToGain(gainGrid[juce::jlimit(0, 14, lfGainIdx)]);

        // 2. BANDE HF (High Shelf ou Cloche)
        static const float hfFreqGrid[] = { 2500.0f, 5000.0f, 7000.0f, 10000.0f, 12500.0f, 16000.0f, 20000.0f };
        float hfFreq = hfFreqGrid[hfFreqIdx];

        typename juce::dsp::IIR::Coefficients<float>::Ptr hfCoeffs;

        if (hfQIdx == 4) // High Shelf
        {
            hfCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, hfFreq, 0.7071f, hfGain);
        }
        else // Cloche
        {
            float hfQ = 1.0f;
            switch (hfQIdx)
            {
            case 0: hfQ = 4.0f; break; // XS
            case 1: hfQ = 2.0f; break; // S
            case 2: hfQ = 1.0f; break; // Normal
            case 3: hfQ = 0.5f; break; // L
            default: break;
            }
            hfCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, hfFreq, hfQ, hfGain);
        }
        *hfBand.state = *hfCoeffs;

        // 3. BANDE MHF
        static const float mhfFreqGrid[] = { 800.0f, 1500.0f, 3000.0f, 5000.0f, 8000.0f, 10000.0f, 12500.0f };
        float mhfFreq = mhfFreqGrid[mhfFreqIdx];
        float mhfQ = 1.0f;

        switch (mhfQIdx)
        {
        case 0: mhfQ = 5.0f; break; // XS
        case 1: mhfQ = 2.5f; break; // S
        case 2: mhfQ = 1.0f; break; // Normal
        case 3: mhfQ = 0.5f; break; // L
        case 4: mhfQ = 0.3f; break; // XL
        default: break;
        }

        auto mhfCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, mhfFreq, mhfQ, mhfGain);
        *mhfBand.state = *mhfCoeffs;

        // 4. BANDE MLF
        static const float mlfFreqGrid[] = { 75.0f, 150.0f, 180.0f, 240.0f, 500.0f, 700.0f, 1000.0f };
        float mlfFreq = mlfFreqGrid[mlfFreqIdx];
        float mlfQ = 1.0f;

        switch (mlfQIdx)
        {
        case 0: mlfQ = 5.0f; break; // XS
        case 1: mlfQ = 2.5f; break; // S
        case 2: mlfQ = 1.0f; break; // Normal
        case 3: mlfQ = 0.5f; break; // L
        case 4: mlfQ = 0.3f; break; // XL
        default: break;
        }

        auto mlfCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, mlfFreq, mlfQ, mlfGain);
        *mlfBand.state = *mlfCoeffs;

        // 5. BANDE LF (Low Shelf ou Cloche)
        static const float lfFreqGrid[] = { 30.0f, 40.0f, 50.0f, 100.0f, 200.0f, 300.0f, 450.0f };
        float lfFreq = lfFreqGrid[lfFreqIdx];

        typename juce::dsp::IIR::Coefficients<float>::Ptr lfCoeffs;

        if (lfQIdx == 4) // Low Shelf
        {
            lfCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, lfFreq, 0.7071f, lfGain);
        }
        else // Cloche
        {
            float lfQ = 1.0f;
            switch (lfQIdx)
            {
            case 0: lfQ = 4.0f; break; // XS
            case 1: lfQ = 2.0f; break; // S
            case 2: lfQ = 1.0f; break; // Normal
            case 3: lfQ = 0.5f; break; // L
            default: break;
            }
            lfCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, lfFreq, lfQ, lfGain);
        }
        *lfBand.state = *lfCoeffs;
    }

    void process(const juce::dsp::ProcessContextReplacing<float>& context)
    {
        lfBand.process(context);
        mlfBand.process(context);
        mhfBand.process(context);
        hfBand.process(context);
    }

private:
    double sampleRate = 44100.0;

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> hfBand;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> mhfBand;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> mlfBand;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lfBand;
};