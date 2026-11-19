#pragma once
#include <JuceHeader.h>

class Clipper
{
public:
    enum class Mode
    {
        Clipper = 0, // Soft Clip Analogique Chaud (Harmoniques Lampe/Bande)
        Limiter      // Brickwall Peak Limiter Transparent (Lookahead 3ms)
    };

    Clipper() = default;
    ~Clipper() = default;

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        numChannels = spec.maximumBlockSize > 0 ? spec.numChannels : 2;

        smoothGainStates.assign(numChannels, 1.0f);
        channelGrDb.assign(numChannels, 0.0f);

        // Fenêtre de Lookahead de 3 ms
        lookaheadSamples = std::max(1, static_cast<int>(sampleRate * 0.003f));

        peakBuffer.assign(lookaheadSamples, 0.0f);
        delayBuffers.resize(numChannels);
        for (auto& buf : delayBuffers)
            buf.assign(lookaheadSamples, 0.0f);

        writePos = 0;

        attackCoeff = std::exp(-4.6f / static_cast<float>(lookaheadSamples));
        releaseCoeff = std::exp(-1.0f / (static_cast<float>(sampleRate) * 0.050f));

        reset();
    }

    void reset()
    {
        std::fill(smoothGainStates.begin(), smoothGainStates.end(), 1.0f);
        std::fill(channelGrDb.begin(), channelGrDb.end(), 0.0f);

        std::fill(peakBuffer.begin(), peakBuffer.end(), 0.0f);
        for (auto& buf : delayBuffers)
            std::fill(buf.begin(), buf.end(), 0.0f);

        writePos = 0;
    }

    // modeIndex    : 0 = CLIP, 1 = LIM
    // ceilingDb    : -20.0f à 0.0f dBFS
    // outputGainDb : -12.0f à +12.0f dB (Gain de sortie propre au module)
    void setParameters(int modeIndex, float ceilingDb, float outputGainDb = 0.0f)
    {
        currentMode = (modeIndex == 0) ? Mode::Clipper : Mode::Limiter;

        float targetDb = (ceilingDb >= 0.0f) ? -0.05f : ceilingDb;
        ceilingGain = juce::Decibels::decibelsToGain(targetDb);
        outputGain = juce::Decibels::decibelsToGain(outputGainDb); // 🟢 Gain de sortie du module
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        const int chCount = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();

        if (static_cast<size_t>(chCount) > smoothGainStates.size())
        {
            smoothGainStates.resize(chCount, 1.0f);
            channelGrDb.resize(chCount, 0.0f);
            delayBuffers.resize(chCount, std::vector<float>(lookaheadSamples, 0.0f));
        }

        if (currentMode == Mode::Clipper)
        {
            // --- MODE CLIPPER ANALOGIQUE CHAUD ---
            for (int ch = 0; ch < chCount; ++ch)
            {
                float* channelData = buffer.getWritePointer(ch);
                float minGainInBlock = 1.0f;

                for (int s = 0; s < numSamples; ++s)
                {
                    float x = channelData[s];
                    float absX = std::abs(x);
                    float y = x;

                    // 🟢 Ne déclenche la saturation QUE si on dépasse le ceiling
                    if (absX > ceilingGain)
                    {
                        float sign = (x > 0.0f) ? 1.0f : -1.0f;
                        float overDb = absX - ceilingGain;

                        // Soft clip progressif uniquement sur le dépassement
                        float saturatedOver = std::tanh(overDb / ceilingGain) * ceilingGain;
                        y = sign * (ceilingGain + saturatedOver);

                        // Sécurité Brickwall au ceiling strict
                        y = juce::jlimit(-ceilingGain, ceilingGain, y);
                    }

                    // Application du Gain de Sortie du module
                    channelData[s] = y * outputGain;

                    // Calcul du GR uniquement si traitement effectif
                    if (absX > 0.00001f)
                    {
                        float sampleGain = std::abs(y) / absX;
                        if (sampleGain < minGainInBlock)
                            minGainInBlock = sampleGain;
                    }
                }

                float grDb = -juce::Decibels::gainToDecibels(minGainInBlock);
                channelGrDb[ch] = std::max(0.0f, grDb);
            }
        }
        else if (currentMode == Mode::Limiter)
        {
            // --- MODE LIMITER TRANSPARENT (LOOKAHEAD 3 MS) ---
            float smoothGain = smoothGainStates[0];
            float minGainInBlock = 1.0f;

            int localWritePos = writePos;
            int readPos = (localWritePos + 1) % lookaheadSamples;

            for (int s = 0; s < numSamples; ++s)
            {
                // 1. Peak instantané Stéréo
                float inPeak = 0.0f;
                for (int ch = 0; ch < chCount; ++ch)
                {
                    float x = buffer.getSample(ch, s);
                    inPeak = std::max(inPeak, std::abs(x));
                    delayBuffers[ch][localWritePos] = x;
                }

                peakBuffer[localWritePos] = inPeak;

                // 2. MAX PEAK dans la fenêtre de 3 ms
                float maxPeakInWindow = 0.0f;
                for (int k = 0; k < lookaheadSamples; ++k)
                    maxPeakInWindow = std::max(maxPeakInWindow, peakBuffer[k]);

                // 3. Gain Cible
                float targetGain = 1.0f;
                if (maxPeakInWindow > ceilingGain && maxPeakInWindow > 0.00001f)
                {
                    targetGain = ceilingGain / maxPeakInWindow;
                }

                // 4. Lissage fluide du gain
                if (targetGain < smoothGain)
                    smoothGain = smoothGain * attackCoeff + targetGain * (1.0f - attackCoeff);
                else
                    smoothGain = smoothGain * releaseCoeff + targetGain * (1.0f - releaseCoeff);

                // 5. Application sur le signal retardé + Output Gain du module
                for (int ch = 0; ch < chCount; ++ch)
                {
                    float delayedSample = delayBuffers[ch][readPos];
                    float y = delayedSample * smoothGain;

                    y = juce::jlimit(-ceilingGain, ceilingGain, y);

                    // 🟢 Application du Gain de Sortie du module
                    buffer.setSample(ch, s, y * outputGain);
                }

                if (smoothGain < minGainInBlock)
                    minGainInBlock = smoothGain;

                localWritePos = (localWritePos + 1) % lookaheadSamples;
                readPos = (localWritePos + 1) % lookaheadSamples;
            }

            writePos = localWritePos;
            smoothGainStates[0] = smoothGain;
            if (chCount > 1) smoothGainStates[1] = smoothGain;

            float grDb = -juce::Decibels::gainToDecibels(minGainInBlock);
            for (int ch = 0; ch < chCount; ++ch)
                channelGrDb[ch] = std::max(0.0f, grDb);
        }
    }

    // --- GETTERS DÉDIÉS AU GRMETER ---
    float getGainReductionLeftDb() const { return !channelGrDb.empty() ? channelGrDb[0] : 0.0f; }
    float getGainReductionRightDb() const { return channelGrDb.size() > 1 ? channelGrDb[1] : getGainReductionLeftDb(); }

private:
    Mode currentMode = Mode::Clipper;
    double sampleRate = 44100.0;
    size_t numChannels = 2;

    float ceilingGain = 0.988f;
    float outputGain = 1.0f; // 🟢 Gain linéaire de sortie (défaut 0 dB = 1.0)
    float attackCoeff = 0.99f;
    float releaseCoeff = 0.999f;

    // Buffers de lookahead & Peak detection
    int lookaheadSamples = 132;
    int writePos = 0;
    std::vector<float> peakBuffer;
    std::vector<std::vector<float>> delayBuffers;

    std::vector<float> smoothGainStates;
    std::vector<float> channelGrDb;
};