#pragma once
#include <JuceHeader.h>

class PiroussSwitch : public juce::Button
{
public:
    // textStateDefault : Affiché quand getToggleState() est false (ex: "K-W" ou "Flat")
    // textStateToggled : Affiché quand getToggleState() est true  (ex: "Flat" ou "Flip")
    PiroussSwitch(const juce::String& textStateDefault, const juce::String& textStateToggled,
        juce::Colour colorDefault, juce::Colour colorToggled);
    ~PiroussSwitch() override = default;

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

private:
    juce::String txtDefault;
    juce::String txtToggled;
    juce::Colour cDefault;
    juce::Colour cToggled;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PiroussSwitch)
};