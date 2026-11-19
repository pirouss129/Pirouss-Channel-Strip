#pragma once
#include <JuceHeader.h>

class PiroussModernButton : public juce::Button
{
public:
	// constructeur qui prend le nom du bouton et la couleur de bordure
    PiroussModernButton(const juce::String& buttonName, juce::Colour activeColour = juce::Colours::white);

    void paintButton(juce::Graphics& g,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override;

private:
    juce::Colour activeColour;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PiroussModernButton)
};