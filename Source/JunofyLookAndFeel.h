#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class JunofyLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    JunofyLookAndFeel();
    ~JunofyLookAndFeel() override;

    juce::Font fontInterRegular(float heightPt) const;
    juce::Font fontInterSemiBold(float heightPt) const;

    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          const juce::Slider::SliderStyle style, juce::Slider& slider) override;

    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY,
                      int buttonW, int buttonH, juce::ComboBox& box) override;

    juce::Font getComboBoxFont(juce::ComboBox& box) override;

    juce::Label* createComboBoxTextBox(juce::ComboBox& box) override;

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& b,
                          bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

private:
    juce::Typeface::Ptr interRegular;
    juce::Typeface::Ptr interSemiBold;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JunofyLookAndFeel)
};
