#pragma once

#include "PluginProcessor.h"

class JunofyAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit JunofyAudioProcessorEditor(JunofyAudioProcessor&);
    ~JunofyAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    JunofyAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JunofyAudioProcessorEditor)
};
