
#include "JunofyEditor.h"
#include "JunofyTheme.h"
#include "BinaryData.h"
#include <juce_audio_processors/juce_audio_processors.h>

namespace
{
struct ParamDef
{
    int id;
    const char* label;
};

static const ParamDef kFxParams[] = {
    { kMasterVol, "Volume" },
    { kEarComfort, "Ear soften" },
    { kFxReverbMix, "Hall" },
    { kFxReverbRoom, "Room" },
    { kFxReverbDamping, "Damp" },
    { kFxReverbWidth, "Width" },
    { kFxWarmth, "Warmth" },
    { kFxTapeWow, "Tape wow" },
    { kMasterComp, "Comp" },
    { kMasterOutGain, "Out" },
    { kMasterLimiter, "Limiter" },
};

static const struct
{
    int id;
    const char* label;
} kChorusIds[] = {
    { kChorusOff, "Off" },
    { kChorusI, "I" },
    { kChorusII, "II" },
};

juce::Image makeGrainTile()
{
    juce::Image img(juce::Image::ARGB, 96, 96, true);
    juce::Random rng(0xC0FFEEu);
    for (int y = 0; y < 96; ++y)
        for (int x = 0; x < 96; ++x)
        {
            const uint8_t a = (uint8_t) rng.nextInt(32);
            img.setPixelAt(x, y, juce::Colour::fromFloatRGBA(1.f, 1.f, 1.f, (float) a / 255.f * 0.14f));
        }
    return img;
}

const juce::Image kGrainTile = makeGrainTile();
} // namespace

void JunofyEditor::layoutSectionUnderline(juce::Label& label, juce::Rectangle<float>& lineOut)
{
    const auto b = label.getBounds().toFloat();
    const float w = juce::jmax(48.f, b.getWidth() * 0.48f);
    lineOut = juce::Rectangle<float>(b.getX(), b.getBottom() + 3.f, w, 1.25f);
}

JunofyEditor::JunofyEditor(KR106AudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setLookAndFeel(&junofyLaf);

    titleLabel.setText("Junofy", juce::dontSendNotification);
    titleLabel.setFont(junofyLaf.fontInterSemiBold(21.0f));
    titleLabel.setColour(juce::Label::textColourId, JunofyTheme::textPrimary());
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(titleLabel);

    subtitleLabel.setText("Factory presets load the Juno patch. Refine chorus, envelope, and space below.",
                          juce::dontSendNotification);
    subtitleLabel.setFont(junofyLaf.fontInterRegular(12.5f));
    subtitleLabel.setColour(juce::Label::textColourId, JunofyTheme::textMuted());
    subtitleLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(subtitleLabel);

    addAndMakeVisible(powerToggle);
    powerToggle.setTooltip("Power: synth on/off.");
    if (auto* rp = dynamic_cast<juce::RangedAudioParameter*>(processor.getParam(kPower)))
    {
        powerAttachment = std::make_unique<juce::ButtonParameterAttachment>(*rp, powerToggle);
        powerAttachment->sendInitialUpdate();
    }

    addAndMakeVisible(presetCombo);
    presetCombo.setTooltip("Presets load the Juno patch; use the controls below for chorus and FX.");
    rebuildPresetList();
    presetCombo.onChange = [this]
    {
        const int idx = presetCombo.getSelectedItemIndex();
        if (idx >= 0)
        {
            processor.setCurrentProgram(idx);
            presetFlash = 1.f;
            repaint();
        }
    };

    presetPrev.onClick = [this]
    {
        const int n = processor.getNumPrograms();
        const int c = processor.getCurrentProgram();
        processor.setCurrentProgram((c - 1 + n) % n);
        syncPresetSelection();
        presetFlash = 1.f;
        repaint();
    };
    presetNext.onClick = [this]
    {
        const int n = processor.getNumPrograms();
        const int c = processor.getCurrentProgram();
        processor.setCurrentProgram((c + 1) % n);
        syncPresetSelection();
        presetFlash = 1.f;
        repaint();
    };
    addAndMakeVisible(presetPrev);
    addAndMakeVisible(presetNext);

    modelCombo.addItem("Juno-60", 1);
    modelCombo.addItem("Juno-106", 2);
    addAndMakeVisible(modelCombo);
    if (auto* par = dynamic_cast<juce::RangedAudioParameter*>(processor.getParam(kAdsrMode)))
    {
        modelAttachment = std::make_unique<juce::ComboBoxParameterAttachment>(*par, modelCombo);
        modelAttachment->sendInitialUpdate();
    }

    transposeToggle.setTooltip("Transpose: MIDI keyboard transposes instead of playing (on-screen keys always play).");
    addAndMakeVisible(transposeToggle);
    if (auto* tp = dynamic_cast<juce::RangedAudioParameter*>(processor.getParam(kTranspose)))
    {
        transposeAttachment = std::make_unique<juce::ButtonParameterAttachment>(*tp, transposeToggle);
        transposeAttachment->sendInitialUpdate();
    }

    auto styleSection = [this](juce::Label& L, const char* text)
    {
        L.setText(text, juce::dontSendNotification);
        L.setFont(junofyLaf.fontInterSemiBold(11.0f));
        L.setColour(juce::Label::textColourId, JunofyTheme::textSection());
        L.setJustificationType(juce::Justification::centredLeft);
    };
    styleSection(sectionChorusLabel, "chorus");
    styleSection(sectionSynthLabel, "envelope");
    styleSection(sectionFxLabel, "space & effects");
    addAndMakeVisible(sectionChorusLabel);
    addAndMakeVisible(sectionSynthLabel);
    addAndMakeVisible(sectionFxLabel);

    for (const auto& ct : kChorusIds)
    {
        auto b = std::make_unique<juce::ToggleButton>(ct.label);
        b->setTooltip("Juno BBD chorus: " + juce::String(ct.label));
        if (auto* rp = dynamic_cast<juce::RangedAudioParameter*>(processor.getParam(ct.id)))
        {
            chorusAttachments.push_back(std::make_unique<juce::ButtonParameterAttachment>(*rp, *b));
            chorusAttachments.back()->sendInitialUpdate();
        }
        addAndMakeVisible(*b);
        chorusToggles.push_back(std::move(b));
    }

    attackLabel.setText("Attack", juce::dontSendNotification);
    attackLabel.setJustificationType(juce::Justification::centredLeft);
    attackLabel.setFont(junofyLaf.fontInterRegular(12.0f));
    attackLabel.setColour(juce::Label::textColourId, JunofyTheme::textLabel());
    attackLabel.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(attackLabel);

    attackSlider = std::make_unique<juce::Slider>();
    attackSlider->setSliderStyle(juce::Slider::LinearHorizontal);
    attackSlider->setTextBoxStyle(juce::Slider::TextBoxRight, false, 54, 22);
    attackSlider->setName("Attack");
    attackSlider->setTooltip("Amp envelope attack time (not LFO delay).");
    addAndMakeVisible(*attackSlider);
    if (auto* rp = dynamic_cast<juce::RangedAudioParameter*>(processor.getParam(kEnvA)))
    {
        attackAttachment = std::make_unique<juce::SliderParameterAttachment>(*rp, *attackSlider);
        attackAttachment->sendInitialUpdate();
    }

    for (const auto& sp : kFxParams)
    {
        auto lab = std::make_unique<juce::Label>(sp.label, sp.label);
        lab->setJustificationType(juce::Justification::centredLeft);
        lab->setFont(junofyLaf.fontInterRegular(12.0f));
        lab->setColour(juce::Label::textColourId, JunofyTheme::textLabel());
        lab->setInterceptsMouseClicks(false, false);
        addAndMakeVisible(*lab);

        auto s = std::make_unique<juce::Slider>();
        s->setSliderStyle(juce::Slider::LinearHorizontal);
        s->setTextBoxStyle(juce::Slider::TextBoxRight, false, 54, 22);
        s->setName(sp.label);

        if (sp.id == kMasterVol)
            s->setTooltip("Master output level.");
        else if (sp.id == kEarComfort)
            s->setTooltip("Softens treble after the synth.");
        else if (sp.id == kFxReverbMix)
            s->setTooltip("Plate reverb amount. 0 = off; otherwise 10%–100% wet.");
        else if (sp.id == kFxReverbRoom || sp.id == kFxReverbDamping || sp.id == kFxReverbWidth)
            s->setTooltip("Plate size, damping, stereo width.");
        else if (sp.id == kFxWarmth)
            s->setTooltip("Soft saturation after FX (output warmth).");
        else if (sp.id == kFxTapeWow)
            s->setTooltip("Slow tape-style pitch wander before echo/reverb.");
        else if (sp.id == kMasterComp)
            s->setTooltip("Bus compressor + makeup: levels quiet presets with loud ones. 0 = off.");
        else if (sp.id == kMasterOutGain)
            s->setTooltip("Output gain after comp (−24…+12 dB). Center ≈ 0 dB.");
        else if (sp.id == kMasterLimiter)
            s->setTooltip("Peak limiter toward 0 dBFS. 0 = off.");
        else
            s->setTooltip(sp.label);

        if (auto* rp = dynamic_cast<juce::RangedAudioParameter*>(processor.getParam(sp.id)))
        {
            fxAttachments.push_back(std::make_unique<juce::SliderParameterAttachment>(*rp, *s));
            fxAttachments.back()->sendInitialUpdate();
        }
        addAndMakeVisible(*s);
        fxLabels.push_back(std::move(lab));
        fxSliders.push_back(std::move(s));
    }

    delayPingPongToggle.setTooltip("Ping-pong echo: short stereo repeats.");
    addAndMakeVisible(delayPingPongToggle);
    if (auto* rp = dynamic_cast<juce::RangedAudioParameter*>(processor.getParam(kFxDelayPingPong)))
    {
        delayPingPongAttachment = std::make_unique<juce::ButtonParameterAttachment>(*rp, delayPingPongToggle);
        delayPingPongAttachment->sendInitialUpdate();
    }

    delayBeforeHallToggle.setTooltip("On: echo then reverb. Off: reverb then echo.");
    addAndMakeVisible(delayBeforeHallToggle);
    if (auto* rp = dynamic_cast<juce::RangedAudioParameter*>(processor.getParam(kFxDelayBeforeReverb)))
    {
        delayBeforeHallAttachment = std::make_unique<juce::ButtonParameterAttachment>(*rp, delayBeforeHallToggle);
        delayBeforeHallAttachment->sendInitialUpdate();
    }

    juce::Image chevron = juce::ImageCache::getFromMemory(BinaryData::transpose_chevron2x_png,
                                                          BinaryData::transpose_chevron2x_pngSize);
    keyboard = std::make_unique<KR106Keyboard>(&processor, chevron);
    addAndMakeVisible(*keyboard);

    lastPresetProgram = processor.getCurrentProgram();
    syncPresetSelection();

    setResizable(true, true);
    setResizeLimits(480, 720, 1200, 960);
    setSize(620, 800);

    startTimerHz(24);
}

JunofyEditor::~JunofyEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void JunofyEditor::rebuildPresetList()
{
    presetCombo.clear(juce::dontSendNotification);
    for (int i = 0; i < processor.getNumPrograms(); ++i)
        presetCombo.addItem(processor.getProgramName(i), i + 1);
}

void JunofyEditor::syncPresetSelection()
{
    const int p = processor.getCurrentProgram();
    presetCombo.setSelectedItemIndex(p, juce::dontSendNotification);
    lastPresetProgram = p;
}

void JunofyEditor::timerCallback()
{
    const int p = processor.getCurrentProgram();
    if (p != lastPresetProgram)
    {
        presetFlash = 1.f;
        syncPresetSelection();
    }

    if (presetFlash > 0.02f)
    {
        presetFlash *= 0.88f;
        repaint();
    }

    if (keyboard != nullptr)
        keyboard->updateFromProcessor();
}

void JunofyEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.fillAll(JunofyTheme::bgApp());

    const float cx = bounds.getCentreX();
    const float cy = bounds.getY() + bounds.getHeight() * 0.20f;
    juce::ColourGradient ambient(
        JunofyTheme::glowIndigo().withAlpha(0.085f), cx, cy,
        JunofyTheme::bgApp().withAlpha(0.f), cx, cy + 260.f, true);
    g.setGradientFill(ambient);
    g.fillRect(bounds);

    auto card = getLocalBounds().toFloat().reduced(18.f, 16.f);
    g.setColour(JunofyTheme::bgCard());
    g.fillRoundedRectangle(card, 11.f);

    {
        juce::Path clip;
        clip.addRoundedRectangle(card, 11.f);
        g.saveState();
        g.reduceClipRegion(clip);
        g.setOpacity(0.042f);
        for (float y = card.getY(); y < card.getBottom(); y += (float) kGrainTile.getHeight())
            for (float x = card.getX(); x < card.getRight(); x += (float) kGrainTile.getWidth())
                g.drawImageAt(kGrainTile, (int) x, (int) y, false);
        g.restoreState();
    }

    g.setColour(JunofyTheme::outlineSubtle().withAlpha(0.55f));
    g.drawRoundedRectangle(card.reduced(0.5f), 11.f, 1.f);

    auto drawLine = [&g](const juce::Rectangle<float>& r)
    {
        g.setColour(JunofyTheme::accent().withAlpha(0.5f));
        g.fillRoundedRectangle(r, r.getHeight() * 0.5f);
    };
    drawLine(underlineChorus);
    drawLine(underlineSynth);
    drawLine(underlineFx);

    if (presetFlash > 0.02f)
    {
        auto tr = titleLabel.getBounds().toFloat().expanded(10, 5);
        g.setColour(JunofyTheme::accent().withAlpha(0.10f * presetFlash));
        g.fillRoundedRectangle(tr, 7.f);
    }
}

void JunofyEditor::resized()
{
    auto r = getLocalBounds().reduced(34, 28);
    titleLabel.setBounds(r.removeFromTop(26));
    r.removeFromTop(6);
    subtitleLabel.setBounds(r.removeFromTop(40));
    r.removeFromTop(16);

    auto bar = r.removeFromTop(36);
    powerToggle.setBounds(bar.removeFromLeft(50));
    bar.removeFromLeft(10);
    presetPrev.setBounds(bar.removeFromLeft(34));
    bar.removeFromLeft(4);
    presetNext.setBounds(bar.removeFromLeft(34));
    bar.removeFromLeft(12);
    {
        const int rest = bar.getWidth();
        const int modelW = juce::jmin(132, rest / 3);
        const int trW = juce::jmin(104, rest / 4);
        presetCombo.setBounds(bar.removeFromLeft(juce::jmax(128, rest - modelW - trW - 16)));
        bar.removeFromLeft(10);
        modelCombo.setBounds(bar.removeFromLeft(modelW));
        bar.removeFromLeft(10);
        transposeToggle.setBounds(bar.removeFromLeft(trW));
    }

    r.removeFromTop(24);
    sectionChorusLabel.setBounds(r.removeFromTop(16));
    layoutSectionUnderline(sectionChorusLabel, underlineChorus);
    r.removeFromTop(8);
    {
        auto row = r.removeFromTop(32);
        const int n = (int) chorusToggles.size();
        const int gap = 10;
        const int tw = (row.getWidth() - (n - 1) * gap) / juce::jmax(1, n);
        for (int i = 0; i < n; ++i)
        {
            if (i > 0)
                row.removeFromLeft(gap);
            chorusToggles[(size_t) i]->setBounds(row.removeFromLeft(tw));
        }
    }

    r.removeFromTop(22);
    sectionSynthLabel.setBounds(r.removeFromTop(16));
    layoutSectionUnderline(sectionSynthLabel, underlineSynth);
    r.removeFromTop(8);
    const int labelW = 104;
    const int rowH = 32;
    {
        auto row = r.removeFromTop(rowH);
        attackLabel.setBounds(row.removeFromLeft(labelW));
        row.removeFromLeft(8);
        attackSlider->setBounds(row);
    }
    r.removeFromTop(22);
    sectionFxLabel.setBounds(r.removeFromTop(16));
    layoutSectionUnderline(sectionFxLabel, underlineFx);
    r.removeFromTop(10);

    const int gapY = 8;
    for (size_t i = 0; i < fxSliders.size(); ++i)
    {
        auto row = r.removeFromTop(rowH);
        fxLabels[i]->setBounds(row.removeFromLeft(labelW));
        row.removeFromLeft(8);
        fxSliders[i]->setBounds(row);
        r.removeFromTop(gapY);
    }

    r.removeFromTop(6);
    {
        auto row = r.removeFromTop(32);
        delayPingPongToggle.setBounds(row.removeFromLeft(juce::jmin(124, row.getWidth() / 2 - 4)));
        row.removeFromLeft(10);
        delayBeforeHallToggle.setBounds(row.removeFromLeft(juce::jmin(200, row.getWidth())));
    }

    r.removeFromTop(14);
    const int keyH = 112;
    auto keyRow = r.removeFromBottom(keyH);
    if (keyboard != nullptr)
        keyboard->setBounds(keyRow.withSizeKeepingCentre(juce::jmin(780, juce::jmax(1, keyRow.getWidth())), keyH));
}
