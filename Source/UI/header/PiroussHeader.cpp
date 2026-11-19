#include "PiroussHeader.h"

// Constructeur
PiroussHeader::PiroussHeader(const juce::String& pluginName, const juce::String& pluginVersion, const juce::Colour themeColor)
    : name(pluginName), version(pluginVersion), color(themeColor)
{
}

// Méthode paint()
void PiroussHeader::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    float margeX = 6.0f;
    float margeY = 3.0f;
    float arrondi = 5.0f;
    auto zoneChrome = bounds.reduced(margeX, margeY);

    // Fond chrome
    juce::ColourGradient chromeGrad(
        color.withAlpha(0.45f), 0.0f, zoneChrome.getY(),
        juce::Colours::black.withAlpha(0.85f), 0.0f, zoneChrome.getBottom(), false);
    g.setGradientFill(chromeGrad);
    g.fillRoundedRectangle(zoneChrome, arrondi);

    // Bordure
    g.setColour(color.withAlpha(1.0f));
    g.drawRoundedRectangle(zoneChrome, arrondi, 1.5f);

    // Logo JUCE (optionnel)
    juce::Image juceLogo = juce::ImageCache::getFromMemory(BinaryData::juce_logo_png, BinaryData::juce_logo_pngSize);
    if (juceLogo.isValid())
    {
        auto logoZone = zoneChrome.removeFromLeft(25.0f).reduced(4.0f);
        g.setOpacity(1.0f);
        g.drawImageWithin(juceLogo,
            logoZone.getX(), logoZone.getY(),
            logoZone.getWidth(), logoZone.getHeight(),
            juce::RectanglePlacement::centred);
        g.setOpacity(1.0f);
    }

    // Titre
    g.setColour(juce::Colours::white.withAlpha(0.9f));
    g.setFont(juce::Font("sans-serif", 13.0f, juce::Font::italic));
	juce::String fullText = name + " v" + version;
    g.drawText(fullText, bounds, juce::Justification::centred, true);
}