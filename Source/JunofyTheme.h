#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

// Reference palette: near-black base, white/cool-gray typography, restrained violet accent (Juno-style marketing site).
// Fonts: use JunofyLookAndFeel::fontInterRegular / fontInterSemiBold (Inter is owned by the L&F, not statics).
namespace JunofyTheme
{
inline juce::Colour bgApp() { return juce::Colour(0xff050506); }
inline juce::Colour bgPanel() { return juce::Colour(0xff0a0a0d); }
inline juce::Colour bgCard() { return juce::Colour(0xff0c0c10); }
inline juce::Colour glowIndigo() { return juce::Colour(0xff5b21b6); }

inline juce::Colour accent() { return juce::Colour(0xffa78bfa); }
inline juce::Colour accentDim() { return juce::Colour(0xff6d28d9); }
inline juce::Colour accentFill() { return accent().withAlpha(0.72f); }

inline juce::Colour textPrimary() { return juce::Colour(0xfffafafa); }
inline juce::Colour textMuted() { return juce::Colour(0xff8b8b98); }
inline juce::Colour textSection() { return juce::Colour(0xffeceaf2); }
inline juce::Colour textLabel() { return juce::Colour(0xffc8c8d4); }

inline juce::Colour surfaceToggleOff() { return juce::Colour(0xff14141a); }
inline juce::Colour surfaceToggleOn() { return juce::Colour(0xff18141f); }
inline juce::Colour outlineSubtle() { return juce::Colour(0xff2e2e36); }
inline juce::Colour outlineFocus() { return accent().withAlpha(0.35f); }

inline juce::Colour comboBg() { return juce::Colour(0xff101014); }
inline juce::Colour editorBg() { return juce::Colour(0xff12121a); }
}
