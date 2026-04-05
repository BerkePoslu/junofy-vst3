cmake_minimum_required(VERSION 3.22)

project(Junofy VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Steinberg VST3 SDK root (folder that contains pluginterfaces/, public.sdk/, etc.)
# Windows: copy the same VST_SDK tree and configure e.g.
#   cmake -B build -G "Visual Studio 17 2022" -A x64 -DVST3_SDK_PATH=C:/Developer/VST_SDK/vst3sdk
if(NOT VST3_SDK_PATH)
    if(APPLE)
        set(VST3_SDK_PATH "/Users/berkeposlu/Developer/VST_SDK/vst3sdk"
            CACHE PATH "Path to Steinberg vst3sdk directory")
    else()
        set(VST3_SDK_PATH "C:/Developer/VST_SDK/vst3sdk"
            CACHE PATH "Path to Steinberg vst3sdk directory")
    endif()
endif()

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.4
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_set_vst3_sdk_path("${VST3_SDK_PATH}")

juce_add_plugin(Junofy
    COMPANY_NAME "Junofy"
    BUNDLE_ID com.junofy.plugin
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Jnfy
    PLUGIN_CODE Jnp1
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "Junofy")

juce_generate_juce_header(Junofy)

target_sources(Junofy
    PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp)

target_compile_definitions(Junofy
    PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(Junofy
    PRIVATE
    juce::juce_audio_utils
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)
