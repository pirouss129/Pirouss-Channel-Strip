#include "PiroussSwitch.h"

PiroussSwitch::PiroussSwitch(const juce::String& textStateDefault, const juce::String& textStateToggled,
    juce::Colour colorDefault, juce::Colour colorToggled)
    : juce::Button(""), txtDefault(textStateDefault), txtToggled(textStateToggled),
    cDefault(colorDefault), cToggled(colorToggled)
{
    // Mode bascule au clic
    setClickingTogglesState(true);
}

void PiroussSwitch::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = getLocalBounds().toFloat();

    // Fond noir complet
    g.fillAll(juce::Colours::black);

    auto btnRect = bounds.reduced(2.0f, 2.0f);
    float cornerSize = 4.0f;

    bool isToggled = !getToggleState();

    // 🎨 1. DÉFINITION DE LA COULEUR ACTIVE DU COMPOSANT
    juce::Colour currentColour = isToggled ? cToggled : cDefault;

    // Fond feutré
    juce::Colour baseColour = currentColour.withAlpha(isToggled ? 0.35f : 0.18f);

    if (shouldDrawButtonAsDown)
        baseColour = baseColour.darker(0.3f);
    else if (shouldDrawButtonAsHighlighted)
        baseColour = baseColour.brighter(0.08f);

    g.setColour(baseColour);
    g.fillRoundedRectangle(btnRect, cornerSize);

    // Contour fin
    g.setColour(currentColour.withAlpha(isToggled ? 0.4f : 0.5f));
    g.drawRoundedRectangle(btnRect, cornerSize, 1.0f);

    // 🎨 2. DESSIN DU TEXTE CENTRÉ (SUR TOUT LE BOUTON)
    g.setFont(juce::FontOptions(11.5f, juce::Font::plain));
    g.setColour(currentColour.brighter(0.15f)); // Couleur du texte

    juce::String textToDraw = isToggled ? txtToggled : txtDefault;

    // On utilise direct btnRect sans trimmedLeft pour utiliser 100% du rectangle !
    g.drawText(textToDraw, btnRect, juce::Justification::centred, true);
}