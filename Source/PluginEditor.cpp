#include "PluginProcessor.h"
#include "PluginEditor.h"

JunofyAudioProcessorEditor::JunofyAudioProcessorEditor(JunofyAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    juce::ignoreUnused(processorRef);
    setSize(400, 300);
}

JunofyAudioProcessorEditor::~JunofyAudioProcessorEditor() = default;

void JunofyAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawFittedText("Junofy", getLocalBounds(), juce::Justification::centred, 1);
}

void JunofyAudioProcessorEditor::resized()
{
}
