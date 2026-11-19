/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/header/PiroussHeader.h"
#include "UI/Bargraph/MiniMeter.h"
#include "UI/Bargraph/GRMeter.h"
#include "UI/Buttons/PiroussSwitch.h"
#include "UI/Buttons/RotativCrantedSlider.h"
#include "UI/Buttons/PiroussRoundSwitch.h"

//==============================================================================
/**
*/
class PiroussChannelStripAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    PiroussChannelStripAudioProcessorEditor (PiroussChannelStripAudioProcessor&);
    ~PiroussChannelStripAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void mouseDoubleClick(const juce::MouseEvent& event) override;
    void mouseDown(const juce::MouseEvent& event) override;

private:
    
    PiroussChannelStripAudioProcessor& audioProcessor;

    bool isInitializing = true;

    // Titre et couleurs de base
    const juce::String pluginTitle = "Pirouss C-Strip";
    const juce::String pluginVersion = JucePlugin_VersionString;
    const juce::Colour colorTitle = juce::Colours::skyblue.withAlpha(0.9f);

    // Instance de l'en-tête du plugin
    PiroussHeader titlePlate{ pluginTitle, pluginVersion, colorTitle };

    const juce::Colour colorBackground = juce::Colours::black;

    // Zones géométriques
    // gauche
    juce::Rectangle<int> headerArea;
    juce::Rectangle<int> leftArea;
    juce::Rectangle<int> inputArea;
    juce::Rectangle<int> filterArea;
    juce::Rectangle<int> eqArea;
    juce::Rectangle<int> HFeqArea;
    juce::Rectangle<int> MHFeqArea;
    juce::Rectangle<int> MLFeqArea;
    juce::Rectangle<int> LFeqArea;
	juce::Rectangle<int> deEsserArea;
    
	// droite
	juce::Rectangle<int> bypassArea; // on n'utilise pas cette zone pour l'instant, mais on la garde au cas où
	juce::Rectangle<int> rightArea;
	juce::Rectangle<int> exp1Area;
    juce::Rectangle<int> exp2Area;
    juce::Rectangle<int> comp1Area;
    juce::Rectangle<int> comp2Area;
	juce::Rectangle<int> colorArea;
    juce::Rectangle<int> colorV2Area;
    juce::Rectangle<int> clipperArea;


    static constexpr int headerH = 30;

    // Gauche : On ajoute 16px aux blocs qui ont un bandeau de titre
    static constexpr int inputH = 101; // (85 + 16)
    static constexpr int filterH = 101; // (85 + 16)

    static constexpr int HFeqH = 105; // (85 + 16 pour le bandeau EQUALIZER + 4 pour aligner)
	static constexpr int MHFeqH = 89;  // 85 + 4 pour aligner
    static constexpr int MLFeqH = 89;  // 85 + 4 pour aligner
    static constexpr int LFeqH = 89;  // 85 + 4 pour aligner

	static constexpr int deEsserH = 101; // (85 + 16)

    // Droite : +16px répartis sur les blocs Expander et Compressor
    static constexpr int exp1H = 93;  // (85 + 8)
    static constexpr int exp2H = 93;  // (85 + 8)

    static constexpr int comp1H = 93;  // (85 + 8)
    static constexpr int comp2H = 93;  // (85 + 8)

	static constexpr int colorH = 101;  // 85 + 16

    static constexpr int colorV2H = 101;  // 85 + 16

	static constexpr int clipperH = 101;  // 85 + 16

    // --- Composants Bypass ---
    PiroussSwitch filterBypassSwitch{ "ON", "OFF", colorTitle, juce::Colours::darkgrey };
    PiroussSwitch eqBypassSwitch{ "ON", "OFF", colorTitle, juce::Colours::darkgrey };
	PiroussSwitch deEsserBypassSwitch{ "ON", "OFF", colorTitle, juce::Colours::darkgrey };
    PiroussSwitch expBypassSwitch{ "ON", "OFF", colorTitle, juce::Colours::darkgrey };
    PiroussSwitch compBypassSwitch{ "ON", "OFF", colorTitle, juce::Colours::darkgrey };
    PiroussSwitch colorBypassSwitch{ "ON", "OFF", colorTitle, juce::Colours::darkgrey };
	PiroussSwitch colorV2BypassSwitch{ "ON", "OFF", colorTitle, juce::Colours::darkgrey };
    PiroussSwitch clipperBypassSwitch{ "ON", "OFF", colorTitle, juce::Colours::darkgrey };

    // --- Composants de la zone Input ---
    // Nom, Taille potard, Couleur, Taille Label Haut, Style Haut, Taille Label Bas, Style Bas
    RotativCrantedSlider inputGainKnob{"GAIN",juce::Colours::grey};
    MiniMeter inputMeter;               // Ton VU-mètre compact
    PiroussRoundSwitch phaseSwitch{ "Phas-inv", "Phas-inv", juce::Colours::chartreuse.withAlpha(0.5f) };

	RotativCrantedSlider highPassKnob{ "HPF",juce::Colours::grey};
	RotativCrantedSlider lowPassKnob{ "LPF",juce::Colours::grey};

    // --- Composants de la zone Eq ---
	RotativCrantedSlider HFFreqKnob{ "HF Freq",juce::Colours::orangered, 24.0f };  
    RotativCrantedSlider HFGainKnob{ "HF Gain",juce::Colours::orangered, 32.0f };
    RotativCrantedSlider HFQKnob{ "HF Q",juce::Colours::orangered, 24.0f };

    RotativCrantedSlider MHFFreqKnob{ "MHF Freq",juce::Colours::forestgreen, 24.0f };
    RotativCrantedSlider MHFGainKnob{ "MHF Gain",juce::Colours::forestgreen, 32.0f };
    RotativCrantedSlider MHFQKnob{ "MHF Q",juce::Colours::forestgreen, 24.0f };

    RotativCrantedSlider MLFFreqKnob{ "MLF Freq",juce::Colours::skyblue, 24.0f };
    RotativCrantedSlider MLFGainKnob{ "MLF Gain",juce::Colours::skyblue, 32.0f };
    RotativCrantedSlider MLFQKnob{ "MLF Q",juce::Colours::skyblue, 24.0f };

    RotativCrantedSlider LFFreqKnob{ "LF Freq",juce::Colours::sandybrown, 24.0f };
    RotativCrantedSlider LFGainKnob{ "LF Gain",juce::Colours::sandybrown, 32.0f };
    RotativCrantedSlider LFQKnob{ "LF Q",juce::Colours::sandybrown, 24.0f };

	// --- Composants de la zone De-Esser ---
	PiroussRoundSwitch deEsserTypeSwitch{ "WIDE", "SPLIT", colorTitle.withAlpha(0.5f) };
	RotativCrantedSlider deEsserFreqKnob{ "DeEsser Freq", juce::Colours::orangered };
	RotativCrantedSlider deEsserThresholdKnob{ "DeEsser Threshold", juce::Colours::orangered };
	GRMeter deEsserGrMeter; // VU-mètre de réduction de gain pour le de-esser

	// --- Composants de la zone Expander ---
	RotativCrantedSlider expThresholdKnob{ "Exp Threshold", juce::Colours::forestgreen };
	RotativCrantedSlider expRatioKnob{ "Exp Ratio", juce::Colours::forestgreen };
	RotativCrantedSlider expRangeKnob{ "Exp Range", juce::Colours::forestgreen };
    RotativCrantedSlider expAttackKnob{ "Exp Attack", juce::Colours::forestgreen };
	RotativCrantedSlider expReleaseKnob{ "Exp Release", juce::Colours::forestgreen };
	GRMeter expGrMeter; // VU-mètre de réduction de gain pour l'expander

    // --- Composants de la zone Compressor ---
    RotativCrantedSlider thresholdKnob{ "Threshold", juce::Colours::cornflowerblue };
    RotativCrantedSlider ratioKnob{ "Ratio", juce::Colours::cornflowerblue };
    RotativCrantedSlider makeupKnob{ "Makeup", juce::Colours::cornflowerblue };
    RotativCrantedSlider attackKnob{ "Attack", juce::Colours::cornflowerblue };
    RotativCrantedSlider releaseKnob{ "Release", juce::Colours::cornflowerblue };
	GRMeter grMeter; // VU-mètre de réduction de gain

	// --- Composants de la zone Color ---
    RotativCrantedSlider colorFreqKnob{ "Freq", juce::Colours::orangered };
    RotativCrantedSlider colorDriveKnob{ "Drive", juce::Colours::orangered };
    RotativCrantedSlider colorMixKnob{ "Mix", juce::Colours::orangered };

	// --- Composants de la zone Color V2 ---
	RotativCrantedSlider colorV2FreqKnob{ "Freq", juce::Colours::orangered };
	RotativCrantedSlider colorV2GainKnob{ "Gain", juce::Colours::orangered };
	RotativCrantedSlider colorV2DriveKnob{ "Drive", juce::Colours::orangered };

	// --- Composants de la zone Clipper ---
	PiroussRoundSwitch clipperSwitch{ "LIM", "CLIP", juce::Colours::gold.withAlpha(0.5f) };
	RotativCrantedSlider clipperCeilingKnob{ "Ceiling", juce::Colours::orangered };
	GRMeter clipperGrMeter; // VU-mètre de réduction de gain pour le clipper
	RotativCrantedSlider outputGainKnob{ "Output Gain", juce::Colours::orangered };

    // ─── ATTACHEMENTS  ───────────────────────────────────────
    // ─── MÉTHODE DE SYNCHRONISATION MANUELLE ────────────────────────────────
    void updateButtonStatesFromParameters();
    
    // --- Attachements (version moderne de JUCE) ---
     
    // BYPASS
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> filterOnAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> eqOnAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> deEsserOnAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> expOnAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> compOnAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> girardinColorOnAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> girardinColorV2OnAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> clipperOnAttachment;

    // INPUT & PHASE
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainAttachment; // 👈 Plus simple et standard
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> phaseAttachment;     // 👈 Plus simple et standard

	// FILTER
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highPassAttachment;  // 👈 Plus simple et standard
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowPassAttachment;   // 👈 Plus simple et standard

    // EQ
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> HFFreqAttachment;   // 👈 Plus simple et standard
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> HFGainAttachment;   // 👈 Plus simple et standard
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> HFQAttachment;   // 👈 Plus simple et standard

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> MHFFreqAttachment;   // 👈 Plus simple et standard
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> MHFGainAttachment;   // 👈 Plus simple et standard
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> MHFQAttachment;   // 👈 Plus simple et standard

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> MLFFreqAttachment;   // 👈 Plus simple et standard
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> MLFGainAttachment;   // 👈 Plus simple et standard
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> MLFQAttachment;   // 👈 Plus simple et standard

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> LFFreqAttachment;   // 👈 Plus simple et standard
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> LFGainAttachment;   // 👈 Plus simple et standard
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> LFQAttachment;   // 👈 Plus simple et standard

    // DE-ESSER
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> deEsserTypeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> deEsserFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> deEsserThresholdAttachment;
    
    // EXPANDER
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> expThresholdAttachment;   // 👈 Plus simple et standard
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> expRatioAttachment;   // 👈 Plus simple et standard
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> expRangeAttachment;   // 👈 Plus simple et standard
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> expAttackAttachment;   // 👈 Plus simple et standard
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> expReleaseAttachment;   // 👈 Plus simple et standard

	// COMPRESSOR
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAttachment;   // 👈 Plus simple et standard
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ratioAttachment;   // 👈 Plus simple et standard
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> makeupAttachment;   // 👈 Plus simple et standard
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;   // 👈 Plus simple et standard
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;   // 👈 Plus simple et standard

	// GIRARDIN COLOR
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> colorFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> colorDriveAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> colorMixAttachment;

	// GIRARDIN COLOR V2
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> colorV2FreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> colorV2GainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> colorV2DriveAttachment;

    // CLIPPER LIMITER
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> clipperTypeAttachment; // 👈 AJOUT ICI
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> clipperCeilingAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainAttachment;

    // Méthode pour dessiner un cadre de section
    void drawSectionGroup(juce::Graphics& g,
            juce::Rectangle<float> bounds,
            const juce::String& title,
            juce::Colour accentColour,
            bool isBypassed = false);

    void updateBypassStates();

	void paintOverChildren(juce::Graphics& g) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PiroussChannelStripAudioProcessorEditor)
};
