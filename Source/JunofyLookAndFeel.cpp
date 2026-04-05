#include "JunofyLookAndFeel.h"
#include "JunofyTheme.h"
#include "BinaryData.h"

JunofyLookAndFeel::JunofyLookAndFeel()
{
    interRegular = juce::Typeface::createSystemTypefaceFor(BinaryData::InterRegular_ttf,
                                                           BinaryData::InterRegular_ttfSize);
    interSemiBold = juce::Typeface::createSystemTypefaceFor(BinaryData::InterSemiBold_ttf,
                                                            BinaryData::InterSemiBold_ttfSize);

    using namespace JunofyTheme;
    setColour(juce::ComboBox::backgroundColourId, comboBg());
    setColour(juce::ComboBox::outlineColourId, outlineSubtle());
    setColour(juce::ComboBox::textColourId, textPrimary());
    setColour(juce::ComboBox::arrowColourId, accent());
    setColour(juce::PopupMenu::backgroundColourId, comboBg());
    setColour(juce::PopupMenu::textColourId, textPrimary());
    setColour(juce::PopupMenu::highlightedBackgroundColourId, surfaceToggleOn());
    setColour(juce::PopupMenu::highlightedTextColourId, textPrimary());
    setColour(juce::TextEditor::textColourId, textPrimary());
    setColour(juce::TextEditor::backgroundColourId, editorBg());
    setColour(juce::TextEditor::outlineColourId, outlineSubtle());
    setColour(juce::Label::textColourId, textPrimary());
    setColour(juce::Slider::thumbColourId, textPrimary());
    setColour(juce::Slider::trackColourId, accentFill());
    setColour(juce::Slider::backgroundColourId, editorBg());
    setColour(juce::TextButton::buttonColourId, surfaceToggleOff());
    setColour(juce::TextButton::buttonOnColourId, surfaceToggleOn());
    setColour(juce::TextButton::textColourOffId, textPrimary());
    setColour(juce::TextButton::textColourOnId, accent());
    setColour(juce::ToggleButton::textColourId, textLabel());
    setColour(juce::ToggleButton::tickColourId, accent());
}

JunofyLookAndFeel::~JunofyLookAndFeel() = default;

juce::Font JunofyLookAndFeel::fontInterRegular(float heightPt) const
{
    if (interRegular != nullptr)
        return juce::Font(juce::FontOptions().withHeight(heightPt).withTypeface(interRegular));
    return juce::Font(juce::FontOptions(heightPt));
}

juce::Font JunofyLookAndFeel::fontInterSemiBold(float heightPt) const
{
    if (interSemiBold != nullptr)
        return juce::Font(juce::FontOptions().withHeight(heightPt).withTypeface(interSemiBold));
    return juce::Font(juce::FontOptions(heightPt, juce::Font::bold));
}

void JunofyLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPos, float minSliderPos, float maxSliderPos,
                                         const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    juce::ignoreUnused(minSliderPos, maxSliderPos);
    if (style != juce::Slider::LinearHorizontal)
    {
        LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        return;
    }

    const float trackH = (float) height * 0.13f;
    const float ty = (float) y + ((float) height - trackH) * 0.5f;
    auto track = juce::Rectangle<float>((float) x, ty, (float) width, trackH);
    g.setColour(slider.findColour(juce::Slider::backgroundColourId));
    g.fillRoundedRectangle(track, trackH * 0.5f);

    const float filledW = juce::jlimit(0.f, track.getWidth(), sliderPos - (float) x);
    auto filled = track.withWidth(filledW);
    g.setColour(slider.findColour(juce::Slider::trackColourId));
    g.fillRoundedRectangle(filled, trackH * 0.5f);

    const float thumbR = (float) height * 0.30f;
    const float cx = juce::jlimit((float) x + thumbR, (float) x + (float) width - thumbR, sliderPos);
    const float cy = (float) y + (float) height * 0.5f;
    g.setColour(juce::Colours::black.withAlpha(0.28f));
    g.fillEllipse(cx - thumbR + 0.8f, cy - thumbR + 1.2f, thumbR * 2.f, thumbR * 2.f);
    g.setColour(slider.findColour(juce::Slider::thumbColourId));
    g.fillEllipse(cx - thumbR, cy - thumbR, thumbR * 2.f, thumbR * 2.f);
    g.setColour(JunofyTheme::outlineSubtle());
    g.drawEllipse(cx - thumbR, cy - thumbR, thumbR * 2.f, thumbR * 2.f, 1.f);
    g.setColour(JunofyTheme::accent().withAlpha(0.45f));
    g.drawEllipse(cx - thumbR + 0.5f, cy - thumbR + 0.5f, thumbR * 2.f - 1.f, thumbR * 2.f - 1.f, 1.f);
}

void JunofyLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& b,
                                         bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    juce::ignoreUnused(shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
    auto area = b.getLocalBounds().toFloat().reduced(0.5f);
    const bool on = b.getToggleState();
    g.setColour(on ? JunofyTheme::surfaceToggleOn() : JunofyTheme::surfaceToggleOff());
    g.fillRoundedRectangle(area, 5.f);
    g.setColour(on ? JunofyTheme::accent().withAlpha(0.42f) : JunofyTheme::outlineSubtle());
    g.drawRoundedRectangle(area, 5.f, 1.f);

    g.setColour(b.findColour(juce::ToggleButton::textColourId));
    g.setFont(fontInterRegular(12.5f));
    g.drawFittedText(b.getButtonText(), b.getLocalBounds().reduced(8, 0), juce::Justification::centred, 2);
}

void JunofyLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY,
                                     int buttonW, int buttonH, juce::ComboBox& box)
{
    juce::ignoreUnused(isButtonDown);
    auto bounds = juce::Rectangle<int>(0, 0, width, height).toFloat().reduced(0.5f);
    g.setColour(box.findColour(juce::ComboBox::backgroundColourId));
    g.fillRoundedRectangle(bounds, 5.f);
    g.setColour(box.findColour(juce::ComboBox::outlineColourId));
    g.drawRoundedRectangle(bounds, 5.f, 1.f);

    juce::Rectangle<int> arrowZone(buttonX, buttonY, buttonW, buttonH);
    juce::Path p;
    p.addTriangle((float) arrowZone.getCentreX() - 3.5f, (float) arrowZone.getCentreY() - 1.5f,
                  (float) arrowZone.getCentreX() + 3.5f, (float) arrowZone.getCentreY() - 1.5f,
                  (float) arrowZone.getCentreX(), (float) arrowZone.getCentreY() + 3.f);
    g.setColour(box.findColour(juce::ComboBox::arrowColourId));
    g.fillPath(p);
}

void JunofyLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                             const juce::Colour& backgroundColour,
                                             bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    juce::ignoreUnused(backgroundColour);
    auto r = button.getLocalBounds().toFloat().reduced(0.5f);
    const float rad = 5.f;
    juce::Colour fill = JunofyTheme::surfaceToggleOff();
    if (shouldDrawButtonAsDown)
        fill = fill.brighter(0.07f);
    else if (shouldDrawButtonAsHighlighted)
        fill = fill.brighter(0.035f);
    g.setColour(fill);
    g.fillRoundedRectangle(r, rad);
    g.setColour(JunofyTheme::outlineSubtle());
    g.drawRoundedRectangle(r, rad, 1.f);
    if (shouldDrawButtonAsHighlighted)
    {
        g.setColour(JunofyTheme::accent().withAlpha(0.22f));
        g.drawRoundedRectangle(r.reduced(0.5f), rad, 1.f);
    }
}

juce::Font JunofyLookAndFeel::getComboBoxFont(juce::ComboBox& box)
{
    juce::ignoreUnused(box);
    return fontInterRegular(13.0f);
}

juce::Label* JunofyLookAndFeel::createComboBoxTextBox(juce::ComboBox& box)
{
    auto* l = LookAndFeel_V4::createComboBoxTextBox(box);
    l->setFont(fontInterRegular(13.0f));
    return l;
}
