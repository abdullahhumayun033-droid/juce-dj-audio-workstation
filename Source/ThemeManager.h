// Ensures this header file is only included once in a single compilation
#pragma once  

// Includes the JUCE framework for GUI styling
#include <JuceHeader.h>

// Defines the ThemeManager class, which handles UI theming for the application
class ThemeManager
{
public:
    // Enum defining available themes
    enum ThemeType { Light, Dark, Custom };

    // Applies the selected theme to the given component
    static void applyTheme(ThemeType theme, juce::Component* component);

    // Returns the background color based on the current theme
    static juce::Colour getBackgroundColor();

    // Returns the button color based on the current theme
    static juce::Colour getButtonColor();

    // Returns the text color based on the current theme
    static juce::Colour getTextColor();

    // New function: Returns the waveform display color based on the theme
    static juce::Colour getWaveformColor();

    // Returns the text color used in the playlist component
    static juce::Colour getPlaylistTextColor();

    // Returns the background color used in the playlist component
    static juce::Colour getPlaylistBackgroundColor();

    //  New function: Returns the background color for text boxes
    static juce::Colour getTextBoxBackgroundColor();

private:
    // Stores the currently active theme
    static ThemeType currentTheme;
};
