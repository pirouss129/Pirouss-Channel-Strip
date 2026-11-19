#pragma once
#include <JuceHeader.h>

// ==============================================================================
// 1. LOOK AND FEEL (Bouton + Valeur sans cadre)
// ==============================================================================
class RotativCrantedLaf : public juce::LookAndFeel_V4
{
public:
    RotativCrantedLaf() = default;

    // --- DESSIN DU BOUTON ROTATIF ---
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
        juce::Slider& slider) override
    {
        // 🟢 Réduction légèrement augmentée (reduced(5.0f)) pour diminuer la taille du potard
        auto bounds = juce::Rectangle<float>(x, y, width, height).reduced(5.0f);

        float diameter = std::min(bounds.getWidth(), bounds.getHeight());
        auto dialBounds = bounds.withSizeKeepingCentre(diameter, diameter);

        float radius = diameter / 2.0f;
        float centreX = dialBounds.getCentreX();
        float centreY = dialBounds.getCentreY();

        // Piste sombre
        juce::Path backgroundArc;
        backgroundArc.addCentredArc(centreX, centreY, radius - 2.0f, radius - 2.0f,
            0.0f, rotaryStartAngle, rotaryEndAngle, true);

        g.setColour(juce::Colour(0xff1e252b));
        g.strokePath(backgroundArc, juce::PathStrokeType(3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Corps du bouton
        g.setColour(juce::Colour(0xff0d1114));
        g.fillEllipse(dialBounds.reduced(3.0f));
        g.setColour(juce::Colours::cyan.withAlpha(0.4f));
        g.drawEllipse(dialBounds.reduced(3.0f), 1.0f);

        // Pointeur
        float currentAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        juce::Path pointer;
        float pointerLength = radius - 4.0f;
        pointer.addRectangle(-1.5f, -pointerLength, 3.0f, pointerLength * 0.55f);
        pointer.applyTransform(juce::AffineTransform::rotation(currentAngle).translated(centreX, centreY));

        g.setColour(juce::Colours::cyan);
        g.fillPath(pointer);

        // Point lumineux
        float dotRadius = 2.0f;
        float dotX = centreX + (radius - 5.0f) * std::sin(currentAngle);
        float dotY = centreY - (radius - 5.0f) * std::cos(currentAngle);
        g.fillEllipse(dotX - dotRadius, dotY - dotRadius, dotRadius * 2.0f, dotRadius * 2.0f);
    }

    // --- CONFIGURATION DE LA POLICE DU TEXTE ---
    juce::Label* createSliderTextBox(juce::Slider& slider) override
    {
        auto* l = LookAndFeel_V4::createSliderTextBox(slider);

        l->setFont(juce::Font(12.0f));
        l->setJustificationType(juce::Justification::centred);
        l->setColour(juce::Label::textColourId, juce::Colours::white);
        l->setColour(juce::Label::textWhenEditingColourId, juce::Colours::cyan);

        // 🟢 ENLÈVE LES BORDURES DU LABEL :
        l->setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
        l->setColour(juce::Label::outlineWhenEditingColourId, juce::Colours::transparentBlack);

        return l;
    }

    // --- TEXTE SEUL PERSONNALISÉ (PAS DE CADRE) ---
    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        // On ignore le dessin par défaut de LookAndFeel_V4 et on gère tout à la main :

        // 1. Récupération de la zone du label
        auto bounds = label.getLocalBounds().toFloat();

        // 2. Personnalisation de la police (taille et style)
        g.setFont(juce::FontOptions(11.0f, juce::Font::plain));

        // 3. Couleur du texte (ex: cyan/couleur de marque, ou juce::Colours::white)
        g.setColour(juce::Colours::white);

        // 4. Dessin du texte centré verticalement et horizontalement
        g.drawText(label.getText(),
            bounds,
            juce::Justification::centred,
            true); // avec ellipse si le texte déborde
    }
};

// ==============================================================================
// 2. COMPOSANT SLIDER
// ==============================================================================
class RotativCrantedSlider : public juce::Component
{
public:
    juce::Slider slider;
    juce::String sliderName;

    RotativCrantedSlider(juce::String name) : sliderName(name)
    {
        slider.setLookAndFeel(&customLaf);
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);

        // Boîte de texte sans bordure d'édition
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 16);
        slider.setNumDecimalPlacesToDisplay(0);

        addAndMakeVisible(slider);
    }

    ~RotativCrantedSlider() override
    {
        slider.setLookAndFeel(nullptr);
    }

    void paint(juce::Graphics& g) override
    {
        // Contour de la tranche
        /*
        g.setColour(juce::Colours::cyan.withAlpha(0.3f));
        g.drawRoundedRectangle(getLocalBounds().toFloat(), 4.0f, 1.0f);
        */
        // Titre
        g.setColour(juce::Colours::white.withAlpha(0.85f));
        g.setFont(juce::Font(12.0f, juce::Font::bold));
        g.drawText(sliderName, getLocalBounds().removeFromTop(18), juce::Justification::centred);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        bounds.removeFromTop(18);
        slider.setBounds(bounds);
    }

    float getValue() const { return static_cast<float>(slider.getValue()); }

private:
    RotativCrantedLaf customLaf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotativCrantedSlider)
};