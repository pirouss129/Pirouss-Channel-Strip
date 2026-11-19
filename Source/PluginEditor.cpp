/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// --- CONSTRUCTEUR ---
//==============================================================================
PiroussChannelStripAudioProcessorEditor::PiroussChannelStripAudioProcessorEditor(PiroussChannelStripAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    isInitializing = true;

    // 1. Rendre visibles tous les composants
    addAndMakeVisible(titlePlate);

    // Rendre visible les modules d'Input
    addAndMakeVisible(inputGainKnob);
    addAndMakeVisible(inputMeter);
    addAndMakeVisible(phaseSwitch);

    addAndMakeVisible(highPassKnob);
    addAndMakeVisible(lowPassKnob);

    addAndMakeVisible(HFFreqKnob);
    addAndMakeVisible(HFGainKnob);
    addAndMakeVisible(HFQKnob);

    addAndMakeVisible(MHFFreqKnob);
    addAndMakeVisible(MHFGainKnob);
    addAndMakeVisible(MHFQKnob);

    addAndMakeVisible(MLFFreqKnob);
    addAndMakeVisible(MLFGainKnob);
    addAndMakeVisible(MLFQKnob);

    addAndMakeVisible(LFFreqKnob);
    addAndMakeVisible(LFGainKnob);
    addAndMakeVisible(LFQKnob);

    addAndMakeVisible(deEsserTypeSwitch);
    addAndMakeVisible(deEsserFreqKnob);
    addAndMakeVisible(deEsserThresholdKnob);
    addAndMakeVisible(deEsserGrMeter);

    addAndMakeVisible(expThresholdKnob);
    addAndMakeVisible(expRatioKnob);
    addAndMakeVisible(expRangeKnob);
    addAndMakeVisible(expAttackKnob);
    addAndMakeVisible(expReleaseKnob);
    addAndMakeVisible(expGrMeter);

    addAndMakeVisible(thresholdKnob);
    addAndMakeVisible(ratioKnob);
    addAndMakeVisible(makeupKnob);
    addAndMakeVisible(attackKnob);
    addAndMakeVisible(releaseKnob);
    addAndMakeVisible(grMeter);

    addAndMakeVisible(colorFreqKnob);
    addAndMakeVisible(colorDriveKnob);
	addAndMakeVisible(colorMixKnob);

    addAndMakeVisible(colorV2FreqKnob);
    addAndMakeVisible(colorV2GainKnob);
    addAndMakeVisible(colorV2DriveKnob);

    addAndMakeVisible(clipperSwitch);
	addAndMakeVisible(clipperCeilingKnob);
	addAndMakeVisible(clipperGrMeter);
	addAndMakeVisible(outputGainKnob);

    // LES BOUTONS BYPASS
    addAndMakeVisible(filterBypassSwitch);
    addAndMakeVisible(eqBypassSwitch);
    addAndMakeVisible(deEsserBypassSwitch);
    addAndMakeVisible(expBypassSwitch);
    addAndMakeVisible(compBypassSwitch);
    addAndMakeVisible(colorBypassSwitch);
    addAndMakeVisible(colorV2BypassSwitch);
    addAndMakeVisible(clipperBypassSwitch);

    // 🟢 LIAISON APVTS <-> BYPASS SWITCHES
    filterOnAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.apvts, "filterOn", filterBypassSwitch);
    eqOnAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.apvts, "eqOn", eqBypassSwitch);
    deEsserOnAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.apvts, "deEsserOn", deEsserBypassSwitch);
    expOnAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.apvts, "expOn", expBypassSwitch);
    compOnAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.apvts, "compOn", compBypassSwitch);
    girardinColorOnAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.apvts, "colorOn", colorBypassSwitch);
    girardinColorV2OnAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.apvts, "colorV2On", colorV2BypassSwitch);
    clipperOnAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.apvts, "clipperOn", clipperBypassSwitch);

    // 🟢 LIAISON APVTS <-> INPUT & PHASE
    inputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "inputGain", inputGainKnob);

    phaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.apvts, "phaseInvert", phaseSwitch);

    // 🟢 LIAISON APVTS <-> FILTRES
    highPassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "highPass", highPassKnob);

    lowPassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "lowPass", lowPassKnob);

    // EQ HF
    HFFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "HFFreq", HFFreqKnob);
    HFGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "HFGain", HFGainKnob);
    HFQAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "HFQ", HFQKnob);

    // EQ MHF
    MHFFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "MHFFreq", MHFFreqKnob);
    MHFGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "MHFGain", MHFGainKnob);
    MHFQAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "MHFQ", MHFQKnob);

    // EQ MLF
    MLFFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "MLFFreq", MLFFreqKnob);
    MLFGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "MLFGain", MLFGainKnob);
    MLFQAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "MLFQ", MLFQKnob);

    // EQ LF
    LFFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "LFFreq", LFFreqKnob);
    LFGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "LFGain", LFGainKnob);
    LFQAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "LFQ", LFQKnob);

    // De-Esser
    deEsserTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.apvts, "deEsserType", deEsserTypeSwitch);
    deEsserFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "deEsserFreq", deEsserFreqKnob);
    deEsserThresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "deEsserThreshold", deEsserThresholdKnob);

    
    // 🟢 LIAISON APVTS <-> EXPANDER
    expThresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "expThreshold", expThresholdKnob);
    expRatioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "expRatio", expRatioKnob);
    expRangeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "expRange", expRangeKnob);
    expAttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "expAttack", expAttackKnob);
    expReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "expRelease", expReleaseKnob);

    // 🟢 LIAISON APVTS <-> COMPRESSOR
    thresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "compThreshold", thresholdKnob);
    ratioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "compRatio", ratioKnob);
    makeupAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "compMakeup", makeupKnob);
    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "compAttack", attackKnob);
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "compRelease", releaseKnob);

    // 🟢 LIAISON APVTS <-> GIRARDIN COLOR
    colorFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "colorFreq", colorFreqKnob);
    colorDriveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "colorDrive", colorDriveKnob);
	colorMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "colorMix", colorMixKnob);

    // 🟢 LIAISON APVTS <-> GIRARDIN COLOR V2
    colorV2FreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "colorV2Freq", colorV2FreqKnob);
    colorV2GainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "colorV2Gain", colorV2GainKnob);
    colorV2DriveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "colorV2Drive", colorV2DriveKnob);

    // ✅ AJOUT DE LA LIAISON POUR CLIP/LIM SWITCH
    clipperTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.apvts, "clipperType", clipperSwitch);

    clipperCeilingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "clipperCeiling", clipperCeilingKnob);

    outputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "outputGain", outputGainKnob);

    // Interceptions et configuration fenêtres
    titlePlate.setInterceptsMouseClicks(false, false);

    // Callbacks lors des clics sur switchs pour forcer la mise à jour graphique instantanée
    filterBypassSwitch.onClick = [this] { updateBypassStates(); };
    eqBypassSwitch.onClick = [this] { updateBypassStates(); };
	deEsserBypassSwitch.onClick = [this] { updateBypassStates(); };
    expBypassSwitch.onClick = [this] { updateBypassStates(); };
    compBypassSwitch.onClick = [this] { updateBypassStates(); };
    colorBypassSwitch.onClick = [this] { updateBypassStates(); };
    colorV2BypassSwitch.onClick = [this] { updateBypassStates(); };
    clipperBypassSwitch.onClick = [this] { updateBypassStates(); };

    // État initial au lancement :
    updateBypassStates();

    setResizable(true, true);
    setResizeLimits(PiroussChannelStripAudioProcessor::minWidth,
        PiroussChannelStripAudioProcessor::minHeight,
        PiroussChannelStripAudioProcessor::maxWidth,
        PiroussChannelStripAudioProcessor::maxHeight);

    // Restaure la dernière taille connue par le Processor
    setSize(audioProcessor.lastUIWidth, audioProcessor.lastUIHeight);

    isInitializing = false;

    // 🟢 DÉMARRER LE TIMER EN TOUTE FIN DE CONSTRUCTEUR
    if (!isTimerRunning())
        startTimerHz(30);
}

//==============================================================================
// --- DESTRUCTEUR ---
//==============================================================================
PiroussChannelStripAudioProcessorEditor::~PiroussChannelStripAudioProcessorEditor()
{
}

//==============================================================================
// --- PAINT ---
//==============================================================================
void PiroussChannelStripAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Fond général
    g.fillAll(colorBackground);

    // --- 1. COULEURS PAR MODULE ---
    auto inputColour = juce::Colours::skyblue;
    auto filterColour = juce::Colours::skyblue;
    auto eqColour = juce::Colours::skyblue;
	auto deEsserColour = juce::Colours::skyblue;
    auto expColour = juce::Colours::skyblue;
    auto compColour = juce::Colours::skyblue;
    auto colorColour = juce::Colours::skyblue;
    auto colorV2Colour = juce::Colours::skyblue;
	auto clipperColour = juce::Colours::skyblue;

    // --- 2. DESSIN DES CADRES ---
    drawSectionGroup(g, inputArea.toFloat(), "INPUT", inputColour);

    drawSectionGroup(g, filterArea.toFloat(), "FILTERS", filterColour, !filterBypassSwitch.getToggleState());

    auto eqFullArea = HFeqArea.getUnion(MHFeqArea).getUnion(MLFeqArea).getUnion(LFeqArea);
    drawSectionGroup(g, eqFullArea.toFloat(), "EQUALIZER", eqColour, !eqBypassSwitch.getToggleState());

    
    drawSectionGroup(g, deEsserArea.toFloat(), "DE-ESSER", deEsserColour, !deEsserBypassSwitch.getToggleState());

    auto expFullArea = exp1Area.getUnion(exp2Area);
    drawSectionGroup(g, expFullArea.toFloat(), "EXPANDER", expColour, !expBypassSwitch.getToggleState());

    auto compFullArea = comp1Area.getUnion(comp2Area);
    drawSectionGroup(g, compFullArea.toFloat(), "COMPRESSOR", compColour, !compBypassSwitch.getToggleState());

    drawSectionGroup(g, colorArea.toFloat(), "DRIVE COLOR", colorColour, !colorBypassSwitch.getToggleState());

	drawSectionGroup(g, colorV2Area.toFloat(), "DRIVE COLOR V2", colorV2Colour, !colorV2BypassSwitch.getToggleState());

	drawSectionGroup(g, clipperArea.toFloat(), "LIMITER / CLIPPER", clipperColour, !clipperBypassSwitch.getToggleState());
}

//==============================================================================
// --- RESIZED ---
//==============================================================================
void PiroussChannelStripAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    audioProcessor.lastUIWidth = bounds.getWidth();
    audioProcessor.lastUIHeight = bounds.getHeight();

    // 1. Header sur TOUTE la largeur tout en haut
    headerArea = bounds.removeFromTop(headerH);
    titlePlate.setBounds(headerArea);

    // 2. Séparation verticale du reste : Gauche (50%) et Droite (50%)
    leftArea = bounds.removeFromLeft(bounds.getWidth() / 2);
    rightArea = bounds;

    // =========================================================================
    // --- COLONNE GAUCHE ---
    // =========================================================================
    inputArea = leftArea.removeFromTop(inputH);
    filterArea = leftArea.removeFromTop(filterH);
    HFeqArea = leftArea.removeFromTop(HFeqH);
    MHFeqArea = leftArea.removeFromTop(MHFeqH);
    MLFeqArea = leftArea.removeFromTop(MLFeqH);
    LFeqArea = leftArea.removeFromTop(LFeqH);
	deEsserArea = leftArea.removeFromTop(deEsserH);

    // --- INPUT AREA ---
    {
        auto cardBounds = inputArea.reduced(3);
        cardBounds.removeFromTop(18); // Titre

        auto colW = cardBounds.getWidth() / 3;
        inputGainKnob.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
        inputMeter.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
        phaseSwitch.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
    }

    // --- FILTER AREA ---
    {
        auto cardBounds = filterArea.reduced(3);
        auto header = cardBounds.removeFromTop(18);

        filterBypassSwitch.setBounds(header.removeFromLeft(36).reduced(2, 1));

        auto colW = cardBounds.getWidth() / 2;
        highPassKnob.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
        lowPassKnob.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
    }

    // --- EQ AREA ---
    {
        auto cardBounds = HFeqArea.reduced(3);
        auto header = cardBounds.removeFromTop(18);

        eqBypassSwitch.setBounds(header.removeFromLeft(36).reduced(2, 1));

        auto colW = cardBounds.getWidth() / 3;
        HFFreqKnob.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
        HFGainKnob.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
        HFQKnob.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
    }
    {
        auto cardBounds = MHFeqArea.reduced(3);
        auto colW = cardBounds.getWidth() / 3;
        MHFFreqKnob.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
        MHFGainKnob.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
        MHFQKnob.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
    }
    {
        auto cardBounds = MLFeqArea.reduced(3);
        auto colW = cardBounds.getWidth() / 3;
        MLFFreqKnob.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
        MLFGainKnob.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
        MLFQKnob.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
    }
    {
        auto cardBounds = LFeqArea.reduced(3);
        auto colW = cardBounds.getWidth() / 3;
        LFFreqKnob.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
        LFGainKnob.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
        LFQKnob.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
    }

	// --- DE-ESSER AREA ---
	{
		auto cardBounds = deEsserArea.reduced(3);
		auto header = cardBounds.removeFromTop(18);
		deEsserBypassSwitch.setBounds(header.removeFromLeft(36).reduced(2, 1));
		auto colW = cardBounds.getWidth() / 4;
		deEsserTypeSwitch.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
		deEsserFreqKnob.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
		deEsserThresholdKnob.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
		deEsserGrMeter.setBounds(cardBounds.reduced(2));
	}

    // =========================================================================
    // --- COLONNE DROITE ---
    // =========================================================================
    exp1Area = rightArea.removeFromTop(exp1H);
    exp2Area = rightArea.removeFromTop(exp2H);

    // --- EXPANDER AREA ---
    {
        auto expFullArea = exp1Area.getUnion(exp2Area).reduced(3);
        auto header = expFullArea.removeFromTop(18);

        expBypassSwitch.setBounds(header.removeFromLeft(36).reduced(2, 1));

        auto row1 = expFullArea.removeFromTop(expFullArea.getHeight() / 2);
        auto colW1 = row1.getWidth() / 3;
        expThresholdKnob.setBounds(row1.removeFromLeft(colW1).reduced(2));
        expRatioKnob.setBounds(row1.removeFromLeft(colW1).reduced(2));
        expRangeKnob.setBounds(row1.removeFromLeft(colW1).reduced(2));

        auto colW2 = expFullArea.getWidth() / 3;
        expAttackKnob.setBounds(expFullArea.removeFromLeft(colW2).reduced(2));
        expReleaseKnob.setBounds(expFullArea.removeFromLeft(colW2).reduced(2));
        expGrMeter.setBounds(expFullArea.removeFromLeft(colW2).reduced(2));
    }

    comp1Area = rightArea.removeFromTop(comp1H);
    comp2Area = rightArea.removeFromTop(comp2H);

    // --- COMPRESSOR AREA ---
    {
        auto compFullArea = comp1Area.getUnion(comp2Area).reduced(3);
        auto header = compFullArea.removeFromTop(18);

        compBypassSwitch.setBounds(header.removeFromLeft(36).reduced(2, 1));

        auto row1 = compFullArea.removeFromTop(compFullArea.getHeight() / 2);
        auto colW1 = row1.getWidth() / 3;
        thresholdKnob.setBounds(row1.removeFromLeft(colW1).reduced(2));
        ratioKnob.setBounds(row1.removeFromLeft(colW1).reduced(2));
        makeupKnob.setBounds(row1.removeFromLeft(colW1).reduced(2));

        auto colW2 = compFullArea.getWidth() / 3;
        attackKnob.setBounds(compFullArea.removeFromLeft(colW2).reduced(2));
        releaseKnob.setBounds(compFullArea.removeFromLeft(colW2).reduced(2));
        grMeter.setBounds(compFullArea.removeFromLeft(colW2).reduced(2));
    }

    // Extraction de Color
    colorArea = rightArea.removeFromTop(colorH);

    // --- COLOR AREA ---
    {
        auto cardBounds = colorArea.reduced(3);
        auto header = cardBounds.removeFromTop(18);

        colorBypassSwitch.setBounds(header.removeFromLeft(36).reduced(2, 1));

        auto colW = cardBounds.getWidth() / 3;
        colorFreqKnob.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
        colorDriveKnob.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
        colorMixKnob.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
    }

    // Extraction de ColorV2
	colorV2Area = rightArea.removeFromTop(colorV2H);

	// --- COLOR V2 AREA ---
	{
		auto cardBounds = colorV2Area.reduced(3);
		auto header = cardBounds.removeFromTop(18);

		colorV2BypassSwitch.setBounds(header.removeFromLeft(36).reduced(2, 1));

		auto colW = cardBounds.getWidth() / 3;
		colorV2FreqKnob.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
		colorV2GainKnob.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
		colorV2DriveKnob.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
	}

    // Extraction de Clipper
    clipperArea = rightArea.removeFromTop(clipperH);

    // --- CLIPPER AREA ---
    {
        auto cardBounds = clipperArea.reduced(3);
        auto header = cardBounds.removeFromTop(18);

        clipperBypassSwitch.setBounds(header.removeFromLeft(36).reduced(2, 1));

        auto colW = cardBounds.getWidth() / 4;
        clipperSwitch.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
        clipperCeilingKnob.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
        clipperGrMeter.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
		outputGainKnob.setBounds(cardBounds.removeFromLeft(colW).reduced(2));
    }
}

//============================================================================
// TIMER CALLBACK
//============================================================================
void PiroussChannelStripAudioProcessorEditor::timerCallback()
{
    updateButtonStatesFromParameters();

    float peakL = audioProcessor.ppmDsp.getPpmLeft();
    float peakR = audioProcessor.ppmDsp.getPpmRight();
    inputMeter.setLevels(peakL, peakR);

	float deEsserGrL = audioProcessor.getDeEsserGainReductionLeftDb();
	float deEsserGrR = audioProcessor.getDeEsserGainReductionRightDb();
	deEsserGrMeter.setLevels(deEsserGrL, deEsserGrR);

    float expGr = audioProcessor.getExpanderGainReductionDb();
    expGrMeter.setLevels(expGr, expGr);

    float compGrL = audioProcessor.getGainReductionLeftDb();
    float compGrR = audioProcessor.getGainReductionRightDb();
    grMeter.setLevels(compGrL, compGrR);

    

    // 🟢 RÉCUPÉRATION DU RABOTAGE CLIPPER/LIMITER
    float clipGrL = audioProcessor.getClipperGainReductionLeftDb();
    float clipGrR = audioProcessor.getClipperGainReductionRightDb();
    clipperGrMeter.setLevels(clipGrL, clipGrR);

    
}

//============================================================================
// DOUBLE CLICK
//============================================================================
void PiroussChannelStripAudioProcessorEditor::mouseDoubleClick(const juce::MouseEvent& event)
{
    if (headerArea.contains(event.getPosition()))
    {
        audioProcessor.lastUIWidth = audioProcessor.PiroussWidth;
        audioProcessor.lastUIHeight = audioProcessor.PiroussHeight;

        setSize(audioProcessor.PiroussWidth, audioProcessor.PiroussHeight);
    }
}

//============================================================================
// MOUSE DOWN
//============================================================================
void PiroussChannelStripAudioProcessorEditor::mouseDown(const juce::MouseEvent& event)
{
    if (event.mods.isPopupMenu())
    {
        if (auto* topWindow = getTopLevelComponent())
        {
            juce::PopupMenu menu;
            bool isCurrentlyTop = topWindow->isAlwaysOnTop();

            menu.addItem(1, "Toujours au-dessus (Always on Top)", true, isCurrentlyTop);

            menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(this),
                [topWindow, isCurrentlyTop](int result)
                {
                    if (result == 1)
                    {
                        topWindow->setAlwaysOnTop(!isCurrentlyTop);
                    }
                });
        }
    }
}

//============================================================================
// METHODE DE CADRES PAR MODULE - DRAW SECTION GROUP
//============================================================================
void PiroussChannelStripAudioProcessorEditor::drawSectionGroup(juce::Graphics& g,
    juce::Rectangle<float> bounds,
    const juce::String& title,
    juce::Colour accentColour,
    bool isBypassed)
{
    auto cardBounds = bounds.reduced(3.0f, 3.0f);
    float cornerSize = 4.0f;

    auto baseBg = juce::Colour(0xff1e1e1e);
    g.setColour(isBypassed ? baseBg.darker(0.4f) : baseBg);
    g.fillRoundedRectangle(cardBounds, cornerSize);

    if (title.isNotEmpty())
    {
        auto titleHeader = cardBounds.removeFromTop(18.0f);

        auto headerBg = isBypassed ? juce::Colours::black.withAlpha(0.2f)
            : accentColour.withAlpha(0.15f);
        g.setColour(headerBg);
        g.fillRoundedRectangle(titleHeader.getX(), titleHeader.getY(),
            titleHeader.getWidth(), titleHeader.getHeight(),
            cornerSize);

        auto textColour = isBypassed ? juce::Colours::grey.withAlpha(0.3f)
            : accentColour.brighter(0.4f);

        auto textBounds = titleHeader.withTrimmedLeft(42.0f);

        g.setFont(juce::FontOptions(11.0f, juce::Font::bold));
        g.setColour(textColour);
        g.drawText(title.toUpperCase(), textBounds,
            juce::Justification::centredLeft, true);
    }
}

void PiroussChannelStripAudioProcessorEditor::updateBypassStates()
{
    // --- FILTERS ---
    const bool filtActive = filterBypassSwitch.getToggleState();
    highPassKnob.setEnabled(filtActive);
    lowPassKnob.setEnabled(filtActive);

    // --- EQUALIZER ---
    const bool eqActive = eqBypassSwitch.getToggleState();
    HFFreqKnob.setEnabled(eqActive);
    HFGainKnob.setEnabled(eqActive);
    HFQKnob.setEnabled(eqActive);

    MHFFreqKnob.setEnabled(eqActive);
    MHFGainKnob.setEnabled(eqActive);
    MHFQKnob.setEnabled(eqActive);

    MLFFreqKnob.setEnabled(eqActive);
    MLFGainKnob.setEnabled(eqActive);
    MLFQKnob.setEnabled(eqActive);

    LFFreqKnob.setEnabled(eqActive);
    LFGainKnob.setEnabled(eqActive);
    LFQKnob.setEnabled(eqActive);

    // --- DE-ESSER ---
    const bool deEsserActive = deEsserBypassSwitch.getToggleState();
    deEsserTypeSwitch.setEnabled(deEsserActive);
    deEsserFreqKnob.setEnabled(deEsserActive);
    deEsserThresholdKnob.setEnabled(deEsserActive);
    deEsserGrMeter.setEnabled(deEsserActive);

    // --- EXPANDER ---
    const bool expActive = expBypassSwitch.getToggleState();
    expThresholdKnob.setEnabled(expActive);
    expRatioKnob.setEnabled(expActive);
    expRangeKnob.setEnabled(expActive);
    expAttackKnob.setEnabled(expActive);
    expReleaseKnob.setEnabled(expActive);
    expGrMeter.setEnabled(expActive);

    // --- COMPRESSOR ---
    const bool compActive = compBypassSwitch.getToggleState();
    thresholdKnob.setEnabled(compActive);
    ratioKnob.setEnabled(compActive);
    makeupKnob.setEnabled(compActive);
    attackKnob.setEnabled(compActive);
    releaseKnob.setEnabled(compActive);
    grMeter.setEnabled(compActive);

    // --- COLOR ---
    const bool colorActive = colorBypassSwitch.getToggleState();
    colorFreqKnob.setEnabled(colorActive);
    colorDriveKnob.setEnabled(colorActive);

	// --- COLOR V2 ---
	const bool colorV2Active = colorV2BypassSwitch.getToggleState();
	colorV2FreqKnob.setEnabled(colorV2Active);
	colorV2GainKnob.setEnabled(colorV2Active);
    colorV2DriveKnob.setEnabled(colorV2Active);

    // --- CLIPPER ---
    const bool clipperActive = clipperBypassSwitch.getToggleState();
    clipperCeilingKnob.setEnabled(clipperActive);
    clipperGrMeter.setEnabled(clipperActive); // 👈 On active/désactive le VU-mètre

    repaint();
}

void PiroussChannelStripAudioProcessorEditor::paintOverChildren(juce::Graphics& g)
{
    // On tronque les 18px du haut pour ne pas assombrir le switch ON/OFF !
    auto drawBypassOverlay = [&g](juce::Rectangle<int> bounds)
        {
            auto cardBounds = bounds.reduced(3).toFloat();
            cardBounds.removeFromTop(18.0f); // Évite de recouvrir le switch dans le header !

            g.setColour(juce::Colours::black.withAlpha(0.45f));
            g.fillRoundedRectangle(cardBounds, 4.0f);
        };

    if (!filterBypassSwitch.getToggleState())
        drawBypassOverlay(filterArea);

    if (!eqBypassSwitch.getToggleState())
    {
        auto eqFullArea = HFeqArea.getUnion(MHFeqArea).getUnion(MLFeqArea).getUnion(LFeqArea);
        drawBypassOverlay(eqFullArea);
    }

    if (!deEsserBypassSwitch.getToggleState())
    {
        drawBypassOverlay(deEsserArea);
    }

    if (!expBypassSwitch.getToggleState())
    {
        auto expFullArea = exp1Area.getUnion(exp2Area);
        drawBypassOverlay(expFullArea);
    }

    if (!compBypassSwitch.getToggleState())
    {
        auto compFullArea = comp1Area.getUnion(comp2Area);
        drawBypassOverlay(compFullArea);
    }

    if (!colorBypassSwitch.getToggleState())
    {
        drawBypassOverlay(colorArea);
    }

    if (!colorV2BypassSwitch.getToggleState())
    {
        drawBypassOverlay(colorV2Area);
    }

    if (!clipperBypassSwitch.getToggleState())
    {
        drawBypassOverlay(clipperArea);
    }

}

void PiroussChannelStripAudioProcessorEditor::updateButtonStatesFromParameters()
{

}