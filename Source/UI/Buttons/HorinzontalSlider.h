#pragma once
#include <JuceHeader.h>

// On crée une petite sous-classe personnalisée de juce::Slider pour contrôler la molette
class SteppedMouseWheelSlider : public juce::Slider
{
public:
    SteppedMouseWheelSlider() : juce::Slider() {}

    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override
    {
        // On bloque le comportement par défaut de JUCE pour éviter les valeurs flottantes bizarres
        // Au lieu de ça, on regarde la direction du défilement
        if (wheel.deltaY != 0.0f)
        {
            float direction = (wheel.deltaY > 0.0f) ? 1.0f : -1.0f;
            float currentValue = static_cast<float>(getValue());

            // On force un déplacement strict de 1 dB par cran de molette
            setValue(currentValue + direction, juce::sendNotificationSync);
        }
    }
};

class HorinzontalSlider : public juce::Component
{
public:
    SteppedMouseWheelSlider slider; // Utilise notre slider personnalisé
	juce::String sliderName;

    HorinzontalSlider(juce::String name) : sliderName(name)
    {
        slider.setSliderStyle(juce::Slider::LinearHorizontal);
        slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);

        // Plage de -10 à +10 par pas de 1.0
        slider.setRange(-10.0, 10.0, 1.0);
        slider.setValue(0.0);

        addAndMakeVisible(slider);
    }

    ~HorinzontalSlider() override = default;

    void paint(juce::Graphics& g) override
    {
        // 1. Facultatif : le petit cadre blanc de test si tu veux voir ses bounds comme les autres
        //g.setColour(juce::Colours::white.withAlpha(0.2f));
        //g.drawRect(getLocalBounds(), 1);

        // 2. 🟢 L'ÉTIQUETTE À GAUCHE
        g.setColour(juce::Colours::white.withAlpha(0.7f));
        g.setFont(juce::Font(13.0f, juce::Font::plain));

        // On dessine le texte aligné à gauche et centré verticalement
        // On lui laisse une largeur fixe de 40 pixels pour ne pas qu'il bave sur le slider
        //g.drawText(sliderName, 5, 0, 55, getHeight(), juce::Justification::centredLeft);
    }

    void resized() override
    {
        // 🟢 On décale le slider de 60 pixels pour laisser le texte tranquille
        //auto sliderArea = getLocalBounds().withTrimmedLeft(100); 
        slider.setBounds(getLocalBounds());
    }

    float getGainValue() const
    {
        return std::round(static_cast<float>(slider.getValue()));
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HorinzontalSlider)
};