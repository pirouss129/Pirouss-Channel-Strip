#pragma once
#include <JuceHeader.h>

class RotativSlider : public juce::Component
{
public:
    juce::Slider slider;
    juce::String sliderName;

    RotativSlider(juce::String name) : sliderName(name)
    {
        // 1. Style rotatif standard
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);

        // 2. On configure la boîte de texte sous le potard (on lui donne de la largeur et de la hauteur)
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow,
            false,
            70,   // Largeur de la boîte de texte
            20);  // Hauteur de la boîte de texte
        
        // 3. 🟢 VALEURS ENTIÈRES : Force l'affichage à 0 décimale dans le carré
        slider.setNumDecimalPlacesToDisplay(0);

        addAndMakeVisible(slider);
    }

    ~RotativSlider() override = default;

    
    void paint(juce::Graphics& g) override
    {
		// DEBUG DESSIN : On trace un contour blanc autour du composant pour visualiser sa zone
        //g.setColour(juce::Colours::white);
        // On trace un rectangle blanc de 1 pixel d'épaisseur tout autour du bord
        //g.drawRect(getLocalBounds(), 1);

        // 2. 🟢 L'ÉTIQUETTE ÉLÉGANTE
        g.setColour(juce::Colours::white.withAlpha(0.7f)); // Blanc un peu adouci
        g.setFont(juce::Font(13.0f, juce::Font::plain));   // Taille sobre

        // On dessine le texte centré tout en haut du rectangle (hauteur de 15px)
        g.drawText(sliderName, 0, 4, getWidth(), 15, juce::Justification::centredTop);
    }

    void resized() override
    {
        // 🟢 On passe de 18 à 28 pixels de marge en haut pour pousser le cercle vers le bas
        auto sliderArea = getLocalBounds().withTrimmedTop(28);
        // 3. On prend TOUT l'espace disponible dans le composant
        slider.setBounds(getLocalBounds());
    }

    float getValue() const { return static_cast<float>(slider.getValue()); }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotativSlider)
};