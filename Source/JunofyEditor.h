#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "JunofyLookAndFeel.h"
#include "Controls/KR106Keyboard.h"

class JunofyEditor final : public juce::AudioProcessorEditor,
                           private juce::Timer
{
public:
    explicit JunofyEditor(KR106AudioProcessor&);
    ~JunofyEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void rebuildPresetList();
    void syncPresetSelection();
    static void layoutSectionUnderline(juce::Label& label, juce::Rectangle<float>& lineOut);

    KR106AudioProcessor& processor;
    JunofyLookAndFeel junofyLaf;

    juce::Label titleLabel;
    juce::Label subtitleLabel;
    juce::ComboBox presetCombo;
    juce::TextButton presetPrev { "<" };
    juce::TextButton presetNext { ">" };
    juce::ComboBox modelCombo;
    juce::ToggleButton transposeToggle { "Transpose" };

    juce::ToggleButton powerToggle { "On" };
    std::unique_ptr<juce::ButtonParameterAttachment> powerAttachment;

    juce::Label sectionOutputLabel;
    juce::Label sectionChorusLabel;
    juce::Label sectionSynthLabel;
    juce::Label sectionFxLabel;

    juce::Rectangle<float> underlineChorus;
    juce::Rectangle<float> underlineSynth;
    juce::Rectangle<float> underlineFx;

    juce::Label attackLabel;
    std::unique_ptr<juce::Slider> attackSlider;
    std::unique_ptr<juce::SliderParameterAttachment> attackAttachment;

    std::vector<std::unique_ptr<juce::ToggleButton>> chorusToggles;
    std::vector<std::unique_ptr<juce::ButtonParameterAttachment>> chorusAttachments;

    std::vector<std::unique_ptr<juce::Label>> fxLabels;
    std::vector<std::unique_ptr<juce::Slider>> fxSliders;
    std::vector<std::unique_ptr<juce::SliderParameterAttachment>> fxAttachments;

    juce::ToggleButton delayPingPongToggle { "Echo" };
    std::unique_ptr<juce::ButtonParameterAttachment> delayPingPongAttachment;
    juce::ToggleButton delayBeforeHallToggle { "Delay→Hall" };
    std::unique_ptr<juce::ButtonParameterAttachment> delayBeforeHallAttachment;

    std::unique_ptr<KR106Keyboard> keyboard;

    std::unique_ptr<juce::ComboBoxParameterAttachment> modelAttachment;
    std::unique_ptr<juce::ButtonParameterAttachment> transposeAttachment;

    int lastPresetProgram = -1;
    float presetFlash = 0.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JunofyEditor)
};
