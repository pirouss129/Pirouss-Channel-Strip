#pragma once

#include <JuceHeader.h>

class RotativCrantedSlider : public juce::Slider,
    public juce::LookAndFeel_V4
{
public:
    // --- CONSTRUCTEUR OPTIMISÉ AVEC TES VALEURS PRÉFÉRÉES PAR DÉFAUT ---
    RotativCrantedSlider(
        const juce::String& name,
        juce::Colour knobColour = juce::Colours::cyan,
		float knobSize = 30.0f,
        juce::Colour backGroundKnobColour = juce::Colours::black.withAlpha(0.8f),
        float topLabelSize = 12.0f,
        juce::Font::FontStyleFlags topStyle = juce::Font::bold,
        float bottomLabelSize = 12.0f,
        juce::Font::FontStyleFlags bottomStyle = juce::Font::plain
    )
        : sliderName(name),
        mainColour(knobColour),
		customKnobSize(knobSize),
        backGroundKnobColour(backGroundKnobColour),
        topFontSize(topLabelSize),
        topFontStyle(topStyle),
        bottomFontSize(bottomLabelSize),
        bottomFontStyle(bottomStyle)
    {
        setLookAndFeel(this);
        setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);

        // Configuration du TextBox du dessous (Valeur)
        setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, static_cast<int>(bottomFontSize + 4.0f));
    }

    ~RotativCrantedSlider() override
    {
        setLookAndFeel(nullptr);
    }

    // --- RENDU DU LABEL DE VALEUR (LABEL DU DESSOUS) ---
    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        auto bounds = label.getLocalBounds().toFloat();

        // Application de la taille et du style du label du dessous (Plain par défaut)
        g.setFont(juce::FontOptions(bottomFontSize, bottomFontStyle));
        g.setColour(juce::Colours::white);

        g.drawText(label.getText(), bounds, juce::Justification::centred, true);
    }

    // --- RENDU DU POTARD ROTATIF ---
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPosProportional, float rotaryStartAngle,
        float rotaryEndAngle, juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();
        auto centreX = bounds.getCentreX();
        auto centreY = bounds.getCentreY();

        // 1. DESSIN DU LABEL DU DESSUS (NOM DU PARAMÈTRE - Bold par défaut)
        g.setFont(juce::FontOptions(topFontSize, topFontStyle));
        g.setColour(juce::Colours::lightgrey);

        auto topLabelBounds = juce::Rectangle<float>(x, y, width, topFontSize + 2.0f);
        g.drawText(sliderName, topLabelBounds, juce::Justification::centred, true);

        // 2. DESSIN DU CORPS DU POTARD (30px par défaut)
        float radius = customKnobSize * 0.5f;
        float knobCentreY = centreY + (topFontSize * 0.5f); // Décalage sous le label du haut

        // Fond du potard
		g.setColour(backGroundKnobColour); // Fond noir avec transparence
        g.fillEllipse(centreX - radius, knobCentreY - radius, radius * 2.0f, radius * 2.0f);

        // Contour actif avec la couleur passée en argument
        g.setColour(mainColour);
        g.drawEllipse(centreX - radius, knobCentreY - radius, radius * 2.0f, radius * 2.0f, 1.5f);

        // 3. AIGUILLE / INDEX
        auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        juce::Path p;
        auto pointerLength = radius * 0.65f;
        auto pointerThickness = 2.0f;

        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, knobCentreY));

        g.fillPath(p);
    }

private:
    juce::String sliderName;
    juce::Colour mainColour;
	
    float customKnobSize;

    juce::Colour backGroundKnobColour;

    float topFontSize;
    juce::Font::FontStyleFlags topFontStyle;

    float bottomFontSize;
    juce::Font::FontStyleFlags bottomFontStyle;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotativCrantedSlider)
};