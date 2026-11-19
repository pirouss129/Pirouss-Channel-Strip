#pragma once
#include <JuceHeader.h>

class PhaseInverter
{
public:
    PhaseInverter() = default;

    // 🔴 Correction ici : juce::dsp::ProcessSpec au lieu de juce::ProcessSpec
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        juce::ignoreUnused(spec);
    }

    // Même chose ici pour le contexte si jamais tu avais une erreur dessus
    void process(const juce::dsp::ProcessContextReplacing<float>& context, bool shouldInvert)
    {
        if (!shouldInvert)
            return;

        auto& block = context.getOutputBlock();
        auto numChannels = block.getNumChannels();
        auto numSamples = block.getNumSamples();

        for (size_t channel = 0; channel < numChannels; ++channel)
        {
            auto* channelData = block.getChannelPointer(channel);

            for (size_t sample = 0; sample < numSamples; ++sample)
            {
                channelData[sample] = -channelData[sample];
            }
        }
    }
};