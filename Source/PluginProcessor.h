/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DSP/PPMDsp.h"
#include "DSP/Gain.h"
#include "DSP/PhaseInverter.h"
#include "DSP/HighPassFilter.h"
#include "DSP/LowPassFilter.h"
#include "DSP/Equalizer.h"
#include "DSP/DeEsser.h"
#include "DSP/Expander.h"
#include "DSP/Compressor.h"
#include "DSP/GirardinColor.h"
#include "DSP/GirardinColorV2.h"
#include "DSP/ClipperLimiter.h"

//==============================================================================
/**
*/
class PiroussChannelStripAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    PiroussChannelStripAudioProcessor();
    ~PiroussChannelStripAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // --- CONSTANTES DE TAILLE ---
    static const int minWidth = 360;
    static const int maxWidth = 600;
    static const int minHeight = 705;
    static const int maxHeight = 800;
    static const int defaultWidth = 360;
    static const int defaultHeight = 705;
    static const int PiroussWidth = 360;
    static const int PiroussHeight = 705;

    // Variables de stockage pour les dimensions de l'UI
    int lastUIWidth = defaultWidth;
    int lastUIHeight = defaultHeight;

    // 🛠️ APVTS PUBLIQUE POUR L'EDITOR
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts;

    // Composant de mesure PPM (Vumètre d'entrée)
    PpmDetector ppmDsp;

    // 🟢 ACCESSEURS POUR LES METERS DE L'EDITOR
    float getExpanderGainReductionDb() const
    {
        return std::abs(expanderModule.getCurrentGainReductionDb());

    }

    float getDeEsserGainReductionLeftDb() const { return deEsserModule.getGainReductionLeftDb(); }
    float getDeEsserGainReductionRightDb() const { return deEsserModule.getGainReductionRightDb(); }

    float getGainReductionLeftDb() const { return compModule.getGainReductionLeftDb(); }
    float getGainReductionRightDb() const { return compModule.getGainReductionRightDb(); }

public:
    // ... tes autres méthodes ...

    // --- GETTERS GR CLIPPER / LIMITER ---
    float getClipperGainReductionLeftDb() const { return clipperModule.getGainReductionLeftDb(); }
    float getClipperGainReductionRightDb() const { return clipperModule.getGainReductionRightDb(); }

private:
    // --- Instances des Modules DSP ---
    PhaseInverter phaseModule;
    Gain inputGainModule;
    HighPassFilter highPassModule;
    LowPassFilter lowPassModule;
    Equalizer eqModule;
	DeEsser deEsserModule;
    PiroussExpander expanderModule;
    AudioCompressor compModule;
    GirardinColor girardinColorModule;
	GirardinColorV2 girardinColorV2Module;
	Clipper clipperModule;
	

    // Stockage thread-safe
    std::atomic<float> expanderGainReductionDb{ 0.0f };

    // --- Pointeurs atomiques pour l'APVTS ---

    // --- BYPASS SWITCHES ---
    std::atomic<float>* filterOnParamPntr = nullptr;
    std::atomic<float>* eqOnParamPntr = nullptr;
    std::atomic<float>* deEsserParamPtr = nullptr;
    std::atomic<float>* expOnParamPntr = nullptr;
    std::atomic<float>* compOnParamPntr = nullptr;
    std::atomic<float>* colorOnParamPntr = nullptr; // 👈 AJOUTÉ pour Girardin Color Bypass
	std::atomic<float>* colorV2OnParamPntr = nullptr; // 👈 AJOUTÉ pour Girardin Color V2 Bypass
    std::atomic<float>* clipperOnParamPntr = nullptr;

    // --- INPUT & PHASE ---
    std::atomic<float>* phaseParamPntr = nullptr;
    std::atomic<float>* inputGainParamPntr = nullptr;

    // --- FILTERS ---
    std::atomic<float>* highPassParamPntr = nullptr;
    std::atomic<float>* lowPassParamPntr = nullptr;

    // --- HF EQ ---
    std::atomic<float>* hfFreqParamPntr = nullptr;
    std::atomic<float>* hfGainParamPntr = nullptr;
    std::atomic<float>* hfQParamPntr = nullptr;

    // --- MHF EQ ---
    std::atomic<float>* mhfFreqParamPntr = nullptr;
    std::atomic<float>* mhfGainParamPntr = nullptr;
    std::atomic<float>* mhfQParamPntr = nullptr;

    // --- MLF EQ ---
    std::atomic<float>* mlfFreqParamPntr = nullptr;
    std::atomic<float>* mlfGainParamPntr = nullptr;
    std::atomic<float>* mlfQParamPntr = nullptr;

    // --- LF EQ ---
    std::atomic<float>* lfFreqParamPntr = nullptr;
    std::atomic<float>* lfGainParamPntr = nullptr;
    std::atomic<float>* lfQParamPntr = nullptr;

	// --- DE-ESSER ---
    std::atomic<float>* deEsserTypeParamPntr = nullptr;
    std::atomic<float>* deEsserFreqgParamPntr = nullptr;
    std::atomic<float>* deEsserThresholdParamPntr = nullptr;

    // --- EXPANDER ---
    std::atomic<float>* expThreshParam = nullptr;
    std::atomic<float>* expRatioParam = nullptr;
    std::atomic<float>* expRangeParam = nullptr;
    std::atomic<float>* expAttackParam = nullptr;
    std::atomic<float>* expReleaseParam = nullptr;

    // --- COMPRESSOR ---
    std::atomic<float>* thresholdParamPntr = nullptr;
    std::atomic<float>* ratioParamPntr = nullptr;
    std::atomic<float>* attackParamPntr = nullptr;
    std::atomic<float>* releaseParamPntr = nullptr;
    std::atomic<float>* makeupGainParamPntr = nullptr;

    // --- GIRARDIN COLOR ---
    std::atomic<float>* colorFreqParamPntr = nullptr;
    std::atomic<float>* colorDriveParamPntr = nullptr;
	std::atomic<float>* colorMixParamPntr = nullptr;

	// --- GIRARDIN COLOR V2 ---
	std::atomic<float>* colorV2FreqParamPntr = nullptr;
	std::atomic<float>* colorV2GainParamPntr = nullptr;
	std::atomic<float>* colorV2DriveParamPntr = nullptr;

	// --- CLIPPER LIMITER ---
    std::atomic<float>* clipperTypeParamPntr = nullptr;
    std::atomic<float>* clipperCeilingParamPntr = nullptr;
    std::atomic<float>* outputGainParamPntr = nullptr;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PiroussChannelStripAudioProcessor)
};