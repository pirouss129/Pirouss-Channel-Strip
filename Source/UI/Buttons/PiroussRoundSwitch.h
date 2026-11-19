#pragma once
#include <JuceHeader.h>

class PiroussRoundSwitch : public juce::Button
{
public:
    // textOff : Affiché à l'état false (ex: "WIDE")
    // textOn  : Affiché à l'état true  (ex: "SPLIT")
    // activeColour : Couleur d'illumination quand le switch est actif (true)
    PiroussRoundSwitch(const juce::String& textOff,
        const juce::String& textOn,
        juce::Colour activeColour);

    ~PiroussRoundSwitch() override = default;

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

private:
    juce::String txtOff;
    juce::String txtOn;
    juce::Colour activeColor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PiroussRoundSwitch)
};