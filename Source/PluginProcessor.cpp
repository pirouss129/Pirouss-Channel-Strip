/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// --- CONSTRUCTEUR ---
//==============================================================================
PiroussChannelStripAudioProcessor::PiroussChannelStripAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
    apvts(*this, nullptr, "Parameters", createParameterLayout()), // 👈 Initialisation ici !
    highPassModule(HighPassFilter::Slope::db12),  // 👈 Instanciation du Coupe-bas avec son Q !
    lowPassModule(LowPassFilter::Slope::db12)    // 👈 Instanciation du Coupe-haut avec son Q !

#else
    : apvts(*this, nullptr, "Parameters", createParameterLayout()), // 👈 Initialisation ici !
    highPassModule(HighPassFilter::Slope::db12),  // 👈 Instanciation du Coupe-bas avec son Q !
    lowPassModule(LowPassFilter::Slope::db12)    // 👈 Instanciation du Coupe-haut avec son Q !
#endif
{
    // ✅ INITIALISATION OBLIGATOIRE (sinon valeurs aléatoires !)
    lastUIWidth = defaultWidth;
    lastUIHeight = defaultHeight;

    // --- Liaison des pointeurs atomiques (Bypass Switche) ---
    filterOnParamPntr = apvts.getRawParameterValue("filterOn");
    eqOnParamPntr = apvts.getRawParameterValue("eqOn");
    deEsserParamPtr = apvts.getRawParameterValue("deEsserOn");
    expOnParamPntr = apvts.getRawParameterValue("expOn");
    compOnParamPntr = apvts.getRawParameterValue("compOn");
    colorOnParamPntr = apvts.getRawParameterValue("colorOn");
    colorV2OnParamPntr = apvts.getRawParameterValue("colorV2On");
    clipperOnParamPntr = apvts.getRawParameterValue("clipperOn");

    // --- Liaison des pointeurs atomiques (Utilitaires & Filtres) ---
    phaseParamPntr = apvts.getRawParameterValue("phaseInvert");
    inputGainParamPntr = apvts.getRawParameterValue("inputGain");
    highPassParamPntr = apvts.getRawParameterValue("highPass");
    lowPassParamPntr = apvts.getRawParameterValue("lowPass");

    // --- Liaison des pointeurs atomiques (Égaliseur 4 bandes) ---
    // Bande HF
    hfFreqParamPntr = apvts.getRawParameterValue("HFFreq");
    hfGainParamPntr = apvts.getRawParameterValue("HFGain");
    hfQParamPntr = apvts.getRawParameterValue("HFQ");

    // Bande MHF
    mhfFreqParamPntr = apvts.getRawParameterValue("MHFFreq");
    mhfGainParamPntr = apvts.getRawParameterValue("MHFGain");
    mhfQParamPntr = apvts.getRawParameterValue("MHFQ");

    // Bande MLF
    mlfFreqParamPntr = apvts.getRawParameterValue("MLFFreq");
    mlfGainParamPntr = apvts.getRawParameterValue("MLFGain");
    mlfQParamPntr = apvts.getRawParameterValue("MLFQ");

    // Bande LF
    lfFreqParamPntr = apvts.getRawParameterValue("LFFreq");
    lfGainParamPntr = apvts.getRawParameterValue("LFGain");
    lfQParamPntr = apvts.getRawParameterValue("LFQ");

	// --- Liaison des pointeurs atomiques (De-Esser) ---
    deEsserTypeParamPntr = apvts.getRawParameterValue("deEsserType");
    deEsserFreqgParamPntr = apvts.getRawParameterValue("deEsserFreq");
    deEsserThresholdParamPntr = apvts.getRawParameterValue("deEsserThreshold");
    
    // --- Liaison des pointeurs atomiques (Expander) ---
    expThreshParam = apvts.getRawParameterValue("expThreshold");
    expRatioParam = apvts.getRawParameterValue("expRatio");
    expRangeParam = apvts.getRawParameterValue("expRange");
    expAttackParam = apvts.getRawParameterValue("expAttack");
    expReleaseParam = apvts.getRawParameterValue("expRelease");

    // --- Liaison des pointeurs atomiques (Compresseur) ---
    thresholdParamPntr = apvts.getRawParameterValue("compThreshold");
    ratioParamPntr = apvts.getRawParameterValue("compRatio");
    attackParamPntr = apvts.getRawParameterValue("compAttack");
    releaseParamPntr = apvts.getRawParameterValue("compRelease");
    makeupGainParamPntr = apvts.getRawParameterValue("compMakeup");

	// --- Liaison des pointeurs atomiques (Girardin Color) ---
    colorFreqParamPntr = apvts.getRawParameterValue("colorFreq");
    colorDriveParamPntr = apvts.getRawParameterValue("colorDrive");
	colorMixParamPntr = apvts.getRawParameterValue("colorMix");

	// --- Liaison des pointeurs atomiques (Girardin Color V2) ---
	colorV2FreqParamPntr = apvts.getRawParameterValue("colorV2Freq");
	colorV2GainParamPntr = apvts.getRawParameterValue("colorV2Gain");
	colorV2DriveParamPntr = apvts.getRawParameterValue("colorV2Drive");

    // --- Liaison des pointeurs atomiques (Clipper / Limiter) ---
    clipperTypeParamPntr = apvts.getRawParameterValue("clipperType");
    clipperCeilingParamPntr = apvts.getRawParameterValue("clipperCeiling");
    outputGainParamPntr = apvts.getRawParameterValue("outputGain");
}

//==============================================================================
// --- DESTRUCTEUR ---
//==============================================================================
PiroussChannelStripAudioProcessor::~PiroussChannelStripAudioProcessor()
{
}

//==============================================================================
const juce::String PiroussChannelStripAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PiroussChannelStripAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool PiroussChannelStripAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool PiroussChannelStripAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double PiroussChannelStripAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PiroussChannelStripAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int PiroussChannelStripAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PiroussChannelStripAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String PiroussChannelStripAudioProcessor::getProgramName(int index)
{
    return {};
}

void PiroussChannelStripAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
// --- PREPARE TO PLAY ---
//==============================================================================
void PiroussChannelStripAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // 1. Initialiser le sample rate interne pour le calcul des constantes de temps
    ppmDsp.prepare(sampleRate);

    // PPM  : Attaque 0ms, Release 0.7s  (génère la chute demandée de -20dB en 0.7s), Hold 0ms
    // QPPM : Attaque 10ms, Release 1.5ss (génère la chute demandée de -20dB en 1.5s), Hold 0ms // pas utilisé.
    ppmDsp.updateParameters(0.0f, 0.7f, 0.0f, 10.0f, 1.5f, 0.0f);

    // On prépare le gain d'entrée
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    inputGainModule.prepare(spec);
    phaseModule.prepare(spec);
    highPassModule.prepare(spec);
    lowPassModule.prepare(spec);

    eqModule.prepare(spec);

	deEsserModule.prepare(spec);

    expanderModule.prepare(spec);

    compModule.prepare(spec);

	girardinColorModule.prepare(spec);

	girardinColorV2Module.prepare(spec);

	clipperModule.prepare(spec);
	
}

//==============================================================================
// --- RELEASE RESSOURCES ---
//==============================================================================
void PiroussChannelStripAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

//==============================================================================
// --- CONFIGURATION DES BUS AUDIO ---
//==============================================================================
#ifndef JucePlugin_PreferredChannelConfigurations
bool PiroussChannelStripAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif


//==============================================================================
// --- PROCESS BLOCK ---
//==============================================================================
void PiroussChannelStripAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // 1. Contexte DSP pour les modules juce::dsp
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    // -----------------------------------------------------------------
    // 2. ÉTAPE D'ENTRÉE : Gain, Phase & Analyse PPM
    // -----------------------------------------------------------------
    if (inputGainParamPntr != nullptr)
    {
        float targetGain = inputGainParamPntr->load();
        inputGainModule.updateParameters(targetGain);
        inputGainModule.process(context);
    }

    // Analyse PPM (Post-Gain)
    ppmDsp.processBlock(buffer);

    // Traitement de la Phase
    if (phaseParamPntr != nullptr)
    {
        bool isPhaseInverted = (phaseParamPntr->load() > 0.5f);
        phaseModule.process(context, isPhaseInverted);
    }

    // -----------------------------------------------------------------
    // 3. 🟢 MODULE FILTRES (HPF / LPF)
    // -----------------------------------------------------------------
    bool filterOn = (filterOnParamPntr != nullptr) ? (filterOnParamPntr->load() > 0.5f) : true;

    if (filterOn && highPassParamPntr != nullptr && lowPassParamPntr != nullptr)
    {
        // --- High Pass Filter (HPF) ---
        int hpfIndex = juce::roundToInt(highPassParamPntr->load());
        if (hpfIndex > 0)
        {
            static const float hpfFreqs[] = { 0.0f, 20.0f, 40.0f, 60.0f, 80.0f, 100.0f, 120.0f, 200.0f, 500.0f };
            highPassModule.updateParameters(hpfFreqs[hpfIndex]);
            highPassModule.process(context);
        }

        // --- Low Pass Filter (LPF) ---
        int lpfIndex = juce::roundToInt(lowPassParamPntr->load());
        if (lpfIndex < 4) // Index 4 = "OFF"
        {
            static const float lpfFreqs[] = { 5000.0f, 8000.0f, 16000.0f, 20000.0f };
            lowPassModule.updateParameters(lpfFreqs[lpfIndex]);
            lowPassModule.process(context);
        }
    }

    // -----------------------------------------------------------------
    // 4. 🟢 MODULE ÉGALISEUR 4 BANDES
    // -----------------------------------------------------------------
    bool eqOn = (eqOnParamPntr != nullptr) ? (eqOnParamPntr->load() > 0.5f) : true;

    if (eqOn && hfFreqParamPntr != nullptr)
    {
        int hfFreq = juce::roundToInt(hfFreqParamPntr->load());
        int hfGain = juce::roundToInt(hfGainParamPntr->load());
        int hfQ = juce::roundToInt(hfQParamPntr->load());

        int mhfFreq = juce::roundToInt(mhfFreqParamPntr->load());
        int mhfGain = juce::roundToInt(mhfGainParamPntr->load());
        int mhfQ = juce::roundToInt(mhfQParamPntr->load());

        int mlfFreq = juce::roundToInt(mlfFreqParamPntr->load());
        int mlfGain = juce::roundToInt(mlfGainParamPntr->load());
        int mlfQ = juce::roundToInt(mlfQParamPntr->load());

        int lfFreq = juce::roundToInt(lfFreqParamPntr->load());
        int lfGain = juce::roundToInt(lfGainParamPntr->load());
        int lfQ = juce::roundToInt(lfQParamPntr->load());

        eqModule.updateParameters(hfFreq, hfGain, hfQ,
            mhfFreq, mhfGain, mhfQ,
            mlfFreq, mlfGain, mlfQ,
            lfFreq, lfGain, lfQ);

        eqModule.process(context);
    }

    // -----------------------------------------------------------------
    // X. 🟢 MODULE DE-ESSER
    // -----------------------------------------------------------------
    bool deEsserOn = (deEsserParamPtr != nullptr) ? (deEsserParamPtr->load() > 0.5f) : false;

    if (deEsserOn && deEsserFreqgParamPntr && deEsserThresholdParamPntr && deEsserTypeParamPntr)
    {
        int freqIdx = juce::roundToInt(deEsserFreqgParamPntr->load());
        int threshIdx = juce::roundToInt(deEsserThresholdParamPntr->load());

        // 🟢 État direct : false (0.0f) = Wide, true (1.0f) = Split
        bool isSplit = (deEsserTypeParamPntr->load() > 0.5f);

        deEsserModule.updateParameters(freqIdx, threshIdx, isSplit);
        deEsserModule.process(context);
    }

    // -----------------------------------------------------------------
    // 5. 🟢 MODULE EXPANDER / GATE
    // -----------------------------------------------------------------
    bool expOn = (expOnParamPntr != nullptr) ? (expOnParamPntr->load() > 0.5f) : true;

    if (expOn && expThreshParam != nullptr && expRatioParam != nullptr &&
        expRangeParam != nullptr && expAttackParam != nullptr && expReleaseParam != nullptr)
    {
        int expThreshIdx = juce::roundToInt(expThreshParam->load());
        int expRatioIdx = juce::roundToInt(expRatioParam->load());
        int expRangeIdx = juce::roundToInt(expRangeParam->load());
        int expAttackIdx = juce::roundToInt(expAttackParam->load());
        int expReleaseIdx = juce::roundToInt(expReleaseParam->load());

        // Conversion des index en valeurs physiques
        float expThreshDb = static_cast<float>(expThreshIdx - 50);
        float expRangeDb = static_cast<float>(expRangeIdx * 4);

        static const float expAttacksMs[] = { 0.1f, 0.5f, 1.0f, 5.0f, 10.0f, 20.0f };
        static const float expReleasesMs[] = { 50.0f, 100.0f, 300.0f, 600.0f, 1200.0f, 2000.0f };

        expanderModule.setParameters(expThreshDb, expRatioIdx, expRangeDb,
            expAttacksMs[expAttackIdx], expReleasesMs[expReleaseIdx]);

        expanderModule.process(buffer);

        // Lecture de la réduction de gain pour le bargraph
        expanderGainReductionDb = std::abs(expanderModule.getCurrentGainReductionDb());
    }
    else
    {
        // RAZ du vumètre si désactivé
        expanderGainReductionDb = 0.0f;
    }

    // -----------------------------------------------------------------
    // 6. 🟢 MODULE COMPRESSEUR
    // -----------------------------------------------------------------
    bool compOn = (compOnParamPntr != nullptr) ? (compOnParamPntr->load() > 0.5f) : true;

    if (compOn && thresholdParamPntr != nullptr)
    {
        int threshIdx = juce::roundToInt(thresholdParamPntr->load());
        int ratioIdx = juce::roundToInt(ratioParamPntr->load());
        int attackIdx = juce::roundToInt(attackParamPntr->load());
        int relIdx = juce::roundToInt(releaseParamPntr->load());
        int makeupIdx = juce::roundToInt(makeupGainParamPntr->load());

        compModule.updateParameters(threshIdx, ratioIdx, attackIdx, relIdx, makeupIdx);
        compModule.process(context);
    }
    

    // -----------------------------------------------------------------
    // 7. 🟢 MODULE GIRARDIN COLOR
    // -----------------------------------------------------------------
    bool colorOn = (colorOnParamPntr != nullptr) ? (colorOnParamPntr->load() > 0.5f) : true;

    if (colorOn && colorFreqParamPntr != nullptr && colorDriveParamPntr != nullptr && colorMixParamPntr != nullptr)
    {
        int freqIdx = juce::roundToInt(colorFreqParamPntr->load());
        int driveIdx = juce::roundToInt(colorDriveParamPntr->load());
        float mixVal = colorMixParamPntr->load();

        girardinColorModule.updateParameters(freqIdx, driveIdx, mixVal);
        girardinColorModule.process(buffer);
    }

    // -----------------------------------------------------------------
    // X. 🟢 MODULE GIRARDIN COLOR V2
    // -----------------------------------------------------------------
    bool colorV2On = (colorV2OnParamPntr != nullptr) ? (colorV2OnParamPntr->load() > 0.5f) : true;

    if (colorV2On && colorV2FreqParamPntr != nullptr && colorV2GainParamPntr != nullptr && colorV2DriveParamPntr != nullptr)
    {
        int freqIdx = juce::roundToInt(colorV2FreqParamPntr->load());
        int gainIdx = juce::roundToInt(colorV2GainParamPntr->load());

        // On charge le Drive/Saturation en float direct (0.0f à 100.0f)
        float driveVal = colorV2DriveParamPntr->load();

        girardinColorV2Module.updateParameters(freqIdx, gainIdx, driveVal);
        girardinColorV2Module.process(buffer);
    }

    // -----------------------------------------------------------------
    // 8. 🟢 CLIPPER / LIMITER
    // -----------------------------------------------------------------
    bool clipperOn = (clipperOnParamPntr != nullptr) ? (clipperOnParamPntr->load() > 0.5f) : false;

    if (clipperOn && clipperTypeParamPntr != nullptr && clipperCeilingParamPntr != nullptr)
    {
        // false (0.0f) = LIM (index 1), true (1.0f) = CLIP (index 0)
        bool isClip = (clipperTypeParamPntr->load() > 0.5f);
        int clipType = isClip ? 0 : 1;

        float clipCeiling = clipperCeilingParamPntr->load();
        float clipOutGain = (outputGainParamPntr != nullptr) ? outputGainParamPntr->load() : 0.0f;

        clipperModule.setParameters(clipType, clipCeiling, clipOutGain);
        clipperModule.process(buffer);
    }
}

//==============================================================================
bool PiroussChannelStripAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PiroussChannelStripAudioProcessor::createEditor()
{
    return new PiroussChannelStripAudioProcessorEditor(*this);
}

//==============================================================================
// --- GET STATE (Sauvegarde)
//==============================================================================
void PiroussChannelStripAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // 1. Récupération de l'état actuel de tous les paramètres
    auto state = apvts.copyState();

    // 2. Ajout des propriétés de taille de fenêtre dans le XML
    state.setProperty("lastUIWidth", lastUIWidth, nullptr);
    state.setProperty("lastUIHeight", lastUIHeight, nullptr);

    // 3. Écriture du tout dans la mémoire binaire du DAW
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

//==============================================================================
// --- SET STATE (Restauration)
//==============================================================================
void PiroussChannelStripAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // 1. Reconstruction du XML depuis les données du DAW
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr)
    {
        if (xmlState->hasTagName(apvts.state.getType()))
        {
            // 2. Restauration des paramètres APVTS
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));

            // 3. Restauration de la taille de l'UI (avec valeurs par défaut de secours)
            lastUIWidth = apvts.state.getProperty("lastUIWidth", defaultWidth);
            lastUIHeight = apvts.state.getProperty("lastUIHeight", defaultHeight);
        }
    }
}

//==============================================================================
// --- APVTS ---
// C'est dans cette fonction que tu vas créer tes paramètres et les ajouter à l'APVTS
//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout PiroussChannelStripAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // ─── BYPASS ZONE ────────────────────────────────────────────────────────
    // Boutons d'activation des modules (Default: true = Module ON)
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ "filterOn", 1 }, "Filters On", true));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ "eqOn", 1 }, "EQ On", true));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ "deEsserOn", 1 }, "DeEsser On", false));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ "expOn", 1 }, "Expander On", false));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ "compOn", 1 }, "Compressor On", true));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ "colorOn", 1 }, "Girardin Color On", false));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ "colorV2On", 1 }, "Girardin Color V2 On", false));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ "clipperOn", 1 }, "Clipper On", false));

    // 1 - Gain d'entrée cranté : -12dB à +12dB, par pas de 1dB, défaut 0dB
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "inputGain", 1 },
        "Input Gain",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 1.0f), // Min, Max, Pas (Step)
        0.0f));                                               // Valeur par défaut

    // 2 - Paramètre de phase (id, nom, valeur par défaut)
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ "phaseInvert", 1 },
        "Phase Invert",
        false)); // Off par défaut

    // 3. 🟢 HPF CRANTÉ : OFF, 20, 40, 60, 80, 100, 120, 200, 500 Hz
    juce::StringArray hpfChoices{ "OFF", "20", "40", "60", "80", "100", "120", "200", "500" };
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "highPass", 1 },
        "High-Pass Freq",
        hpfChoices,
        0)); // Par défaut sur l'index 0 (c'est-à-dire "OFF")

    // 4. 🟢 LPF CRANTÉ : 5K, 8K, 16K, 20K, OFF
    juce::StringArray lpfChoices{ "5K", "8K", "16K", "20K", "OFF" };
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "lowPass", 1 },
        "Low-Pass Freq",
        lpfChoices,
        4)); // Par défaut sur l'index 4 (c'est-à-dire "OFF")

    // ─── GRILLES DE PARAMÈTRES PARTAGÉES POUR L'EQ ──────────────────────────

    // Grille de Gain commune : -12, -9, -6, -4, -2, 0, 2, 4, 6, 9, 12
    juce::StringArray eqGainChoices{ "-20", "-15", "-12", "-9", "-6", "-4", "-2", "0", "+2", "+4", "+6", "+9", "+12", "+15", "+20" };
    const int defaultGainIndex = 7; // Index de "0" dans le tableau ci-dessus


    // ─── 5. BANDE HF (HIGH FREQUENCY) ───────────────────────────────────────
    juce::StringArray hfFreqChoices{ "2.5K", "5K", "7K", "10K", "12.5K", "16K", "20K" };
    juce::StringArray hfQChoices{ "XS", "Serré", "Normal", "L", "plateau" };

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "HFFreq", 1 }, "HF Freq", hfFreqChoices, 3)); // Par défaut : 10K (index 3)

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "HFGain", 1 }, "HF Gain", eqGainChoices, defaultGainIndex)); // Par défaut : 0 dB (index 5)

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "HFQ", 1 }, "HF Q", hfQChoices, 2)); // Par défaut : Normal (index 2)


    // ─── 6. BANDE MHF (MID-HIGH FREQUENCY) ──────────────────────────────────
    juce::StringArray mhfFreqChoices{ "800", "1.5K", "3K", "5K", "8K", "10K", "12.5K" };
    juce::StringArray mhfQChoices{ "XS", "S", "Normal", "L", "XL" }; // 5 choix

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "MHFFreq", 1 }, "MHF Freq", mhfFreqChoices, 3)); // Par défaut : 5K (index 3)

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "MHFGain", 1 }, "MHF Gain", eqGainChoices, defaultGainIndex)); // Par défaut : 0 dB

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "MHFQ", 1 }, "MHF Q", mhfQChoices, 2)); // Par défaut : Normal (index 2)


    // ─── 7. BANDE MLF (MID-LOW FREQUENCY) ───────────────────────────────────
    juce::StringArray mlfFreqChoices{ "75", "150", "180", "240", "500", "700", "1K" };
    juce::StringArray mlfQChoices{ "XS", "S", "Normal", "L", "XL" }; // 5 choix

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "MLFFreq", 1 }, "MLF Freq", mlfFreqChoices, 3)); // Par défaut : 240 (index 3)

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "MLFGain", 1 }, "MLF Gain", eqGainChoices, defaultGainIndex)); // Par défaut : 0 dB

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "MLFQ", 1 }, "MLF Q", mlfQChoices, 2)); // Par défaut : Normal (index 2)


    // ─── 8. BANDE LF (LOW FREQUENCY) ────────────────────────────────────────
    juce::StringArray lfFreqChoices{ "30", "40", "50", "100", "200", "300", "450" };
    juce::StringArray lfQChoices{ "XS", "S", "Normal", "L", "Plateau" };

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "LFFreq", 1 }, "LF Freq", lfFreqChoices, 3)); // Par défaut : 100 (index 3)

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "LFGain", 1 }, "LF Gain", eqGainChoices, defaultGainIndex)); // Par défaut : 0 dB

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "LFQ", 1 }, "LF Q", lfQChoices, 2)); // Par défaut : Normal (index 2)

    // ─── DE-ESSER ─────────────────────────────────────────────────────────────

    // false = Wide (0), true = Split (1)
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ "deEsserType", 1 },
        "DeEsser Type",
        false)); // false = Wide par défaut !

    // Freq par défaut (ex: 6 kHz, index 2)
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "deEsserFreq", 1 },
        "DeEsser Freq",
        juce::StringArray{ "3k", "4.5k", "6k", "7.5k", "9k", "11k", "13k" },
        2));

    // Threshold : -80 dB à 0 dB (0 = OFF / Bypass)
    juce::StringArray deEsserThreshChoices;
    for (int db = -80; db <= 0; ++db)
    {
        if (db == 0)
            deEsserThreshChoices.add("OFF");
        else
            deEsserThreshChoices.add(juce::String(db) + " dB");
    }

    // Default à l'index 40 = -40 dB (bouton centré à 12h)
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "deEsserThreshold", 1 },
        "DeEsser Threshold",
        deEsserThreshChoices,
        40)); // 40 = -40 dB par défaut

    

    // ─── 9. EXPANDER / GATE ─────────────────────────────────────────────────

    // Threshold : -50 dBFS à 0 dBFS (51 positions)
    juce::StringArray expThreshChoices;
    for (int db = -50; db <= 0; ++db)
    {
        expThreshChoices.add(juce::String(db) + " dB");
    }

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "expThreshold", 1 },
        "Exp Threshold",
        expThreshChoices,
        0)); // Valeur par défaut : index 0 (-50 dB)

    // Ratio : 0 = OFF (1:1), 1 = 1:2, 2 = 1:4, 3 = 1:8, 4 = GATE
    juce::StringArray expRatioChoices{ "OFF", "1:2", "1:4", "1:8", "GATE" };
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "expRatio", 1 }, "Exp Ratio", expRatioChoices, 0)); // Défaut : OFF (index 0)

    // Range : 0 dB à -40 dB par pas de 4 dB (11 positions)
    juce::StringArray expRangeChoices;
    for (int db = 0; db <= 40; db += 4)
        expRangeChoices.add("-" + juce::String(db) + " dB");
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "expRange", 1 }, "Exp Range", expRangeChoices, 5)); // Défaut : -20 dB (index 5)

    // Attack : 0.1ms, 0.5ms, 1ms, 5ms, 10ms, 20ms
    juce::StringArray expAttackChoices{ "0.1 ms", "0.5 ms", "1 ms", "5 ms", "10 ms", "20 ms" };
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "expAttack", 1 }, "Exp Attack", expAttackChoices, 1)); // Défaut : 0.5 ms (index 1)

    // Release : 50ms, 100ms, 300ms, 600ms, 1.2s, 2s
    juce::StringArray expReleaseChoices{ "50 ms", "100 ms", "300 ms", "600 ms", "1.2 s", "2 s" };
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "expRelease", 1 }, "Exp Release", expReleaseChoices, 2)); // Défaut : 300 ms (index 2)


    // ─── 10. COMPRESSEUR ─────────────────────────────────────────────────────

    // Threshold : -40 dB à 0 dB (0 = OFF / Bypass)
    juce::StringArray threshChoices;
    for (int db = -40; db <= 0; ++db)
    {
        if (db == 0)
            threshChoices.add("OFF");
        else
            threshChoices.add(juce::String(db) + " dB");
    }
    // Index 40 correspond à "OFF" (0 dBFS) par défaut
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "compThreshold", 1 }, "Comp Threshold", threshChoices, 40));

    // Ratio : 2, 3, 4, 8, 12, 20
    juce::StringArray ratioChoices{ "2:1", "3:1", "4:1", "8:1", "12:1", "20:1" };
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "compRatio", 1 }, "Comp Ratio", ratioChoices, 2)); // Défaut : 4:1 (index 2)

    // Attack : 20us, 100us, 800us, 10ms, 30ms
    juce::StringArray attackChoices{ "0,02 ms", "0,1 ms", "0,8 ms", "10 ms", "30 ms" };
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "compAttack", 1 }, "Comp Attack", attackChoices, 1)); // Défaut : 0,1 ms (index 1)

    // Release : 50ms, 100ms, 300ms, 600ms, 1.2s, Auto
    juce::StringArray releaseChoices{ "50 ms", "100 ms", "300 ms", "600 ms", "1.2 s", "Auto" };
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "compRelease", 1 }, "Comp Release", releaseChoices, 2)); // Défaut : 300 ms (index 2)

    // Makeup Gain : 0 dB à +18 dB par pas de 1 dB
    juce::StringArray makeupChoices;
    for (int db = 0; db <= 18; ++db)
        makeupChoices.add(juce::String(db) + " dB");
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "compMakeup", 1 }, "Comp Makeup", makeupChoices, 0)); // Défaut : 0 dB (index 0)

    // ─── 11. GIRARDIN COLOR ──────────────────────────────────────────────────
    // 8 Positions Fréquence cibles (Sub à Air)
    juce::StringArray colorFreqChoices{ "40 Hz", "80 Hz", "200 Hz", "700 Hz", "1.5 kHz", "3.2 kHz", "6.5 kHz", "12 kHz" };
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "colorFreq", 1 }, "Girardin Freq", colorFreqChoices, 4)); // Défaut : 1.5 kHz (Présence Girardin - Index 4)

    // 11 Positions Drive (0 = Clean/Bypass interne, 10 = Saturation féroce)
    juce::StringArray colorDriveChoices{ "0 (Clean)", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" };
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "colorDrive", 1 }, "Girardin Drive", colorDriveChoices, 0)); // Défaut : 0

    // Paramètre Mix cranté par pas de 10% (0%, 10%, 20%... 100%), par défaut à 100%
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "colorMix", 1 },
        "Color Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 10.0f), // 🟢 Pas de 10%
        100.0f)); // 🟢 100% par défaut (à fond à droite)

    // ─── XX. GIRARDIN COLOR V2 ──────────────────────────────────────────────────
    // 8 Positions Fréquence cibles (Sub à Air)
    juce::StringArray colorV2FreqChoices{ "40 Hz", "80 Hz", "200 Hz", "700 Hz", "1.5 kHz", "3.2 kHz", "6.5 kHz", "12 kHz" };
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "colorV2Freq", 1 },
        "Girardin V2 Freq",
        colorV2FreqChoices,
        4)); // Défaut : 1.5 kHz (Présence Girardin - Index 4)

    // 11 Positions Gain (0 à 10)
    juce::StringArray colorV2GainChoices{ "0 (Clean)", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" };
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ "colorV2Gain", 1 },
        "Girardin V2 Gain",
        colorV2GainChoices,
        0)); // Défaut : 0

    // Paramètre Drive cranté par pas de 10% (0% à 100%), par défaut à 100%
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "colorV2Drive", 1 },
        "Girardin V2 Drive",
        juce::NormalisableRange<float>(0.0f, 100.0f, 10.0f), // Pas de 10%
        0.0f)); // 0% par défaut

	// - 12. CLIPPER LIMITER ─────────────────────────────────────────────────────────────
    // 1. Bouton Switch CLIP / LIM
    // false = LIM (défaut), true = CLIP
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ "clipperType", 1 },
        "Clipper Type",
        false)); // false = Limiter par défaut

    // 2. Potard cranté de 0 dB à -20 dB par pas de 1 dB
    layout.add(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID{ "clipperCeiling", 1 }, "Clipper Ceiling",
        -20, 0, 0)); // Min: -20, Max: 0, Defaut: 0

    // 3. Output Gain (-12 dB à +12 dB, pas de 1 dB, défaut 0 dB)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "outputGain", 1 },
        "Output Gain",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 1.0f), // Min, Max, Pas
        0.0f));                                               // Défaut

    return layout;
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PiroussChannelStripAudioProcessor();
}
