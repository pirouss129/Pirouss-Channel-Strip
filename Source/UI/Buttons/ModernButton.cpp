#include "ModernButton.h"

PiroussModernButton::PiroussModernButton(const juce::String& buttonName, juce::Colour activeColour)
    : juce::Button(buttonName), activeColour(activeColour)
{
    setClickingTogglesState(true); // Bouton toggle
}

void PiroussModernButton::paintButton(juce::Graphics& g,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    auto bounds = getLocalBounds().toFloat().reduced(1.0f);
    float cornerRadius = 4.0f;
    bool isActive = getToggleState();

    // Fond métallique
    juce::ColourGradient grad(
        juce::Colours::darkgrey.withAlpha(0.15f), 0.0f, 0.0f,
        juce::Colours::black.withAlpha(0.4f), 0.0f, (float)getHeight(), false);
    g.setGradientFill(grad);
    g.fillRoundedRectangle(bounds, cornerRadius);

    // Bordure selon l'état
    if (isActive)
    {
        g.setColour(activeColour.withAlpha(0.8f));
        g.drawRoundedRectangle(bounds, cornerRadius, 1.5f);
        g.setColour(juce::Colours::white);
    }
    else if (shouldDrawButtonAsHighlighted) // Hover
    {
        g.setColour(juce::Colours::white.withAlpha(0.25f));
        g.drawRoundedRectangle(bounds, cornerRadius, 1.0f);
        g.setColour(juce::Colours::white.withAlpha(0.7f));
    }
    else // Repos
    {
        g.setColour(juce::Colours::white.withAlpha(0.08f));
        g.drawRoundedRectangle(bounds, cornerRadius, 1.0f);
        g.setColour(juce::Colours::white.withAlpha(0.4f));
    }

    // Texte
    g.setFont(juce::Font("sans-serif", (float)getHeight() * 0.45f, juce::Font::bold));
    g.drawText(getButtonText(), getLocalBounds(), juce::Justification::centred, true);
}