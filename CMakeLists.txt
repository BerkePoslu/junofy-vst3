cmake_minimum_required(VERSION 3.22)
project(Junofy VERSION 2.4.8)

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum macOS version")

add_subdirectory(libs/JUCE)
add_subdirectory(libs/clap-juce-extensions EXCLUDE_FROM_ALL)

# LV2 on macOS runs juce_lv2_helper in post-build; it can segfault on some Apple toolchains.
if(APPLE)
    set(JUNOFY_FORMATS AU VST3 Standalone)
else()
    set(JUNOFY_FORMATS VST3 LV2 Standalone)
endif()
option(JUNOFY_COPY_AFTER_BUILD "Copy plugins to system directories after build" ON)

juce_add_plugin(Junofy
    COMPANY_NAME "Junofy"
    COMPANY_WEBSITE "https://github.com/junofy"
    BUNDLE_ID com.junofy.plugin
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    PLUGIN_MANUFACTURER_CODE Jnfy
    PLUGIN_CODE Jnp1
    PLUGIN_DESCRIPTION "Juno-6/60/106 engine (Ultramaster KR-106) — Junofy"
    FORMATS ${JUNOFY_FORMATS}
    PRODUCT_NAME "Junofy"
    PLUGIN_NAME "Junofy"
    ICON_BIG "${CMAKE_CURRENT_SOURCE_DIR}/resources/img/icon_1024.png"
    ICON_SMALL "${CMAKE_CURRENT_SOURCE_DIR}/resources/img/icon_1024.png"
    LV2URI "https://junofy.local/plugin"
    COPY_PLUGIN_AFTER_BUILD ${JUNOFY_COPY_AFTER_BUILD}
)

juce_add_binary_data(JunofyBinaryData SOURCES
    resources/img/kr106_background@2x.png
    resources/img/knob@2x.png
    resources/img/smallknob@2x.png
    resources/img/switch_3way@2x.png
    resources/img/switch_3way_horizontal@2x.png
    resources/img/led_red@2x.png
    resources/img/kr106_bender_gradient@2x.png
    resources/img/transpose_chevron@2x.png
    resources/img/new_knob@2x.png
    resources/img/slider_handle@2x.png
    resources/fonts/Segment14.otf
    resources/fonts/Inter-Regular.ttf
    resources/fonts/Inter-SemiBold.ttf
)

target_sources(Junofy PRIVATE
    Source/PluginProcessor.cpp
    Source/JunofyEditor.cpp
    Source/JunofyLookAndFeel.cpp
    Source/PluginProcessor.h
    Source/JunofyEditor.h
    Source/JunofyLookAndFeel.h
    Source/JunofyTheme.h
    Source/KR106PresetManager.h
    Source/KR106_Presets_JUCE.h
    Source/Controls/KR106Bender.h
    Source/Controls/KR106Button.h
    Source/Controls/KR106Keyboard.h
    Source/Controls/KR106MenuSheet.h
    Source/Controls/KR106Knob.h
    Source/Controls/KR106Misc.h
    Source/Controls/KR106PresetDisplay.h
    Source/Controls/KR106Scope.h
    Source/Controls/KR106Slider.h
    Source/Controls/KR106Switch.h
    Source/Controls/KR106Tooltip.h
    Source/Controls/KR106VarianceSheet.h
    Source/DSP/BBDFilter.h
    Source/DSP/KR106ADSR.h
    Source/DSP/KR106Arpeggiator.h
    Source/DSP/KR106Chorus.h
    Source/DSP/KR106LFO.h
    Source/DSP/KR106Noise.h
    Source/DSP/KR106VCF.h
    Source/DSP/KR106VcfFreqJ106.h
    Source/DSP/KR106VcfFreqJ6.h
    Source/DSP/KR106Voice.h
    Source/DSP/KR106_DSP.h
    Source/DSP/KR106_DSP_SetParam.h
    Source/DSP/KR106_HPF.h
)

target_include_directories(Junofy PRIVATE
    Source
    Source/DSP
    Source/Controls
)

target_compile_definitions(Junofy PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    _USE_MATH_DEFINES
)

target_link_libraries(Junofy PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    JunofyBinaryData
)

clap_juce_extensions_plugin(TARGET Junofy
    CLAP_ID "org.junofy.junofy"
    CLAP_FEATURES instrument synthesizer analog
)

if(APPLE)
    juce_add_plugin(JunofySysEx
        COMPANY_NAME "Junofy"
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT TRUE
        NEEDS_MIDI_OUTPUT TRUE
        AU_MAIN_TYPE "kAudioUnitType_MusicEffect"
        PLUGIN_MANUFACTURER_CODE Jnfy
        PLUGIN_CODE JnSx
        FORMATS AU
        PRODUCT_NAME "Junofy (SysEx)"
        PLUGIN_NAME "Junofy (SysEx)"
        ICON_BIG "${CMAKE_CURRENT_SOURCE_DIR}/resources/img/icon_1024.png"
        ICON_SMALL "${CMAKE_CURRENT_SOURCE_DIR}/resources/img/icon_1024.png"
        COPY_PLUGIN_AFTER_BUILD ${JUNOFY_COPY_AFTER_BUILD}
    )

    target_sources(JunofySysEx PRIVATE
        Source/PluginProcessor.cpp
        Source/JunofyEditor.cpp
        Source/JunofyLookAndFeel.cpp
        Source/PluginProcessor.h
        Source/JunofyEditor.h
        Source/JunofyLookAndFeel.h
        Source/JunofyTheme.h
        Source/KR106PresetManager.h
        Source/KR106_Presets_JUCE.h
        Source/Controls/KR106Bender.h
        Source/Controls/KR106Button.h
        Source/Controls/KR106Keyboard.h
        Source/Controls/KR106MenuSheet.h
        Source/Controls/KR106Knob.h
        Source/Controls/KR106Misc.h
        Source/Controls/KR106PresetDisplay.h
        Source/Controls/KR106Scope.h
        Source/Controls/KR106Slider.h
        Source/Controls/KR106Switch.h
        Source/Controls/KR106Tooltip.h
        Source/Controls/KR106VarianceSheet.h
        Source/DSP/BBDFilter.h
        Source/DSP/KR106ADSR.h
        Source/DSP/KR106Arpeggiator.h
        Source/DSP/KR106Chorus.h
        Source/DSP/KR106LFO.h
        Source/DSP/KR106Noise.h
        Source/DSP/KR106VCF.h
        Source/DSP/KR106VcfFreqJ106.h
        Source/DSP/KR106VcfFreqJ6.h
        Source/DSP/KR106Voice.h
        Source/DSP/KR106_DSP.h
        Source/DSP/KR106_DSP_SetParam.h
        Source/DSP/KR106_HPF.h
    )

    target_include_directories(JunofySysEx PRIVATE
        Source
        Source/DSP
        Source/Controls
    )

    target_compile_definitions(JunofySysEx PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        _USE_MATH_DEFINES
        KR106_AUMF_BUILD=1
    )

    target_link_libraries(JunofySysEx PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        JunofyBinaryData
    )
endif()

if(APPLE)
    set(CUSTOM_ICNS "${CMAKE_CURRENT_SOURCE_DIR}/resources/img/KR106.icns")
    add_custom_command(TARGET Junofy_Standalone POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${CUSTOM_ICNS}"
            "$<TARGET_BUNDLE_DIR:Junofy_Standalone>/Contents/Resources/Icon.icns"
    )
endif()
