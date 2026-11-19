#pragma once
#include <JuceHeader.h>

class GRMeter : public juce::Component
{
public:
    GRMeter() = default;
    ~GRMeter() override = default;

    void setLevels(float grLeftDb, float grRightDb)
    {
        // Attaque instantanée / Ballistique douce en release (lissage visuel)
        smoothGrL = std::max(grLeftDb, smoothGrL * 0.82f);
        smoothGrR = std::max(grRightDb, smoothGrR * 0.82f);

        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();

        // Fond sombre du boîtier de mesure
        g.setColour(juce::Colours::black.withAlpha(0.7f));
        g.fillRoundedRectangle(bounds, 4.0f);
        g.setColour(juce::Colours::darkgrey);
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

        auto innerArea = bounds.reduced(4.0f);

        // Découpe en 2 colonnes (Gauche / Droite)
        auto colWidth = (innerArea.getWidth() - 4.0f) / 2.0f;
        auto leftCol = innerArea.removeFromLeft(colWidth);
        innerArea.removeFromLeft(4.0f); // Espace central entre L et R
        auto rightCol = innerArea;

        // Dessin des échelles de LEDs L et R
        drawLedRack(g, leftCol, smoothGrL);
        drawLedRack(g, rightCol, smoothGrR);
    }

private:
    float smoothGrL = 0.0f;
    float smoothGrR = 0.0f;

    // Définition des couleurs des 9 LEDs selon tes seuils :
    // 0-3 (V), 3-6 (V), 6-9 (V), 9-12 (J), 12-15 (J), 15-18 (J), 18-21 (J), 21-24 (R), >24 (R)
    juce::Colour getLedColor(int ledIndex) const
    {
        if (ledIndex < 3) return juce::Colours::lime;    // LEDs 1, 2, 3 : Vertes
        if (ledIndex < 7) return juce::Colours::yellow;  // LEDs 4, 5, 6, 7 : Jaunes
        return juce::Colours::red;                       // LEDs 8, 9 : Rouges
    }

    void drawLedRack(juce::Graphics& g, juce::Rectangle<float> area, float grDb)
    {
        static constexpr int numLeds = 9;
        float spacing = 2.0f;
        float ledHeight = (area.getHeight() - (spacing * (numLeds - 1))) / static_cast<float>(numLeds);

        for (int i = 0; i < numLeds; ++i)
        {
            auto ledRect = juce::Rectangle<float>(
                area.getX(),
                area.getY() + static_cast<float>(i) * (ledHeight + spacing),
                area.getWidth(),
                ledHeight
            );

            juce::Colour baseColor = getLedColor(i);

            // --- SEUIL D'ALLUMAGE PAR LED ---
            // LED 0 (première LED) : s'allume à partir de 0.1 dB de réduction
            // LEDs 1 à 8 : s'allument à 3 dB, 6 dB, 9 dB, etc.
            float minThresholdDb = (i == 0) ? 0.1f : static_cast<float>(i) * 3.0f;

            if (grDb >= minThresholdDb)
            {
                // LED ALLUMÉE
                g.setColour(baseColor);
                g.fillRoundedRectangle(ledRect, 1.5f);
            }
            else
            {
                // LED ÉTEINTE (Sombre)
                g.setColour(baseColor.darker(0.85f).withAlpha(0.3f));
                g.fillRoundedRectangle(ledRect, 1.5f);
            }
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GRMeter)
};