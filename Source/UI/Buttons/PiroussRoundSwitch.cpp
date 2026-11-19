#include "PiroussRoundSwitch.h"

PiroussRoundSwitch::PiroussRoundSwitch(const juce::String& textOff,
    const juce::String& textOn,
    juce::Colour activeColour)
    : juce::Button(""),
    txtOff(textOff),
    txtOn(textOn),
    activeColor(activeColour)
{
    setClickingTogglesState(true);
}

void PiroussRoundSwitch::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    const bool state = getToggleState();
    auto bounds = getLocalBounds().toFloat();

    const float diameter = std::min(bounds.getWidth(), bounds.getHeight()) - 4.0f;
    auto circleArea = juce::Rectangle<float>(diameter, diameter).withCentre(bounds.getCentre());

    juce::Colour baseColor = state ? activeColor : juce::Colour(0xff1a1d20);

    if (shouldDrawButtonAsDown)
        baseColor = baseColor.darker(0.3f);
    else if (shouldDrawButtonAsHighlighted)
        baseColor = baseColor.brighter(0.18f);

    // 🎨 1. FOND NOIR D'ANCRAGE
    g.setColour(juce::Colours::black);
    g.fillEllipse(circleArea);

    // 🎨 2. DÉGRADÉ RADIAL EXAGÉRÉ (EFFET BOMBÉ / SPHERIQUE ACCENTUÉ)
    // Center beaucoup plus lumineux (+0.45f) et bord nettement assombri (-0.4f)
    juce::Colour centerColor = baseColor.brighter(state ? 0.45f : 0.22f);
    juce::Colour outerColor = baseColor.darker(state ? 0.40f : 0.50f);

    juce::ColourGradient grad(centerColor, circleArea.getCentreX(), circleArea.getCentreY(),
        outerColor, circleArea.getCentreX(), circleArea.getY(), true);
    g.setGradientFill(grad);
    g.fillEllipse(circleArea);

    // 🎨 3. HALO LUMINEUX INTERNE (SI ALLUMÉ)
    if (state)
    {
        auto innerRing = circleArea.reduced(2.0f);
        g.setColour(centerColor.withAlpha(0.35f));
        g.drawEllipse(innerRing, 1.5f);
    }

    // 🎨 4. CONTOUR EXTERNE MÉTALLIQUE / STRUCTURÉ
    juce::Colour borderColor = state ? activeColor.brighter(0.6f).withAlpha(0.85f)
        : juce::Colour(0xff4a535a).withAlpha(0.6f);
    g.setColour(borderColor);
    g.drawEllipse(circleArea, 1.2f);

    // 🎨 5. TYPOGRAPHIE ET NIVEAU DE LISIBILITÉ
    g.setFont(juce::FontOptions(11.5f, juce::Font::bold));

    juce::Colour textColor = state ? juce::Colours::white
        : juce::Colour(0xff99a0a6);

    g.setColour(textColor);

    const juce::String textToDraw = state ? txtOn : txtOff;
    g.drawText(textToDraw, circleArea, juce::Justification::centred, true);
}