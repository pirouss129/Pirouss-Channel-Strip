#pragma once
#include <JuceHeader.h>

class MiniMeter : public juce::Component
{
public:
    MiniMeter() = default;
    ~MiniMeter() override = default;

    // Structure représentant une LED physique de l'afficheur
    struct LedSegment
    {
        float minDb;
        float maxDb;
        juce::Colour colour;
    };

    void setLevels(float newLeftDb, float newRightDb)
    {
        leftDb = newLeftDb;
        rightDb = newRightDb;
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();

        // Fond sombre identique au boîtier du GRMeter
        g.setColour(juce::Colours::black.withAlpha(0.7f));
        g.fillRoundedRectangle(bounds, 4.0f);
        g.setColour(juce::Colours::darkgrey);
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

        auto innerArea = bounds.reduced(3.0f);

        // Définition précise des 9 segments de LEDs (du haut vers le bas)
        const std::vector<LedSegment> ledScale = {
            {   0.0f,  99.0f, juce::Colours::magenta }, // Au-dessus de 0 dB (Sécurité)
            {  -3.0f,   0.0f, juce::Colours::red },           // Led 0 à -3
            {  -6.0f,  -3.0f, juce::Colours::red },           // Led -3 à -6
            {  -9.0f,  -6.0f, juce::Colours::orange },        // Led -6 à -9
            { -12.0f,  -9.0f, juce::Colours::yellow },        // Led -9 à -12
            { -15.0f, -12.0f, juce::Colours::yellow },        // Led -12 à -15
            { -18.0f, -15.0f, juce::Colours::yellow },        // Led -15 à -18
            { -24.0f, -18.0f, juce::Colours::green },         // Led -18 à -24
            { -42.0f, -24.0f, juce::Colours::green }          // Led -24 à -42
        };

        const int numLeds = static_cast<int>(ledScale.size());

        // Séparation géométrique Gauche / Droite
        float paddingBetweenChannels = 3.0f;
        float channelWidth = (innerArea.getWidth() - paddingBetweenChannels) / 2.0f;

        // Calcul de la hauteur d'une LED individuelle
        float totalHeight = innerArea.getHeight();
        float paddingBetweenLeds = 1.5f;
        float ledHeight = (totalHeight - (paddingBetweenLeds * (numLeds - 1))) / numLeds;

        // Parcours de chaque segment
        for (int i = 0; i < numLeds; ++i)
        {
            const auto& led = ledScale[i];

            // Calcul de la position Y
            float ledY = innerArea.getY() + i * (ledHeight + paddingBetweenLeds);

            // Rectangles géométriques des LEDs L et R
            auto leftLedRect = juce::Rectangle<float>(innerArea.getX(), ledY, channelWidth, ledHeight);
            auto rightLedRect = juce::Rectangle<float>(innerArea.getX() + channelWidth + paddingBetweenChannels, ledY, channelWidth, ledHeight);

            // --- 1. ÉTAT CANAL GAUCHE ---
            if (leftDb > led.minDb)
            {
                g.setColour(led.colour);
                g.fillRoundedRectangle(leftLedRect, 1.5f);
            }
            else
            {
                // MÊME STYLE QUE LE GRMETER POUR LA LED ÉTEINTE
                g.setColour(led.colour.darker(0.85f).withAlpha(0.3f));
                g.fillRoundedRectangle(leftLedRect, 1.5f);
            }

            // --- 2. ÉTAT CANAL DROIT ---
            if (rightDb > led.minDb)
            {
                g.setColour(led.colour);
                g.fillRoundedRectangle(rightLedRect, 1.5f);
            }
            else
            {
                g.setColour(led.colour.darker(0.85f).withAlpha(0.3f));
                g.fillRoundedRectangle(rightLedRect, 1.5f);
            }
        }
    }

private:
    float leftDb = -60.0f;
    float rightDb = -60.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MiniMeter)
};