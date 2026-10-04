#include "ThemeManager.h"

// **Initialize Default Theme (Light Mode)**
ThemeManager::ThemeType ThemeManager::currentTheme = ThemeManager::Light;

// **Get Background Color for Text Boxes Based on Current Theme**
juce::Colour ThemeManager::getTextBoxBackgroundColor()
{
    return (currentTheme == Dark) ? juce::Colours::darkgrey :  // Dark mode uses dark grey
        (currentTheme == Custom) ? juce::Colours::hotpink : // Custom mode uses hot pink
        juce::Colours::white; // Default (Light mode) uses white
}

// **Apply Selected Theme to the Application**
void ThemeManager::applyTheme(ThemeType theme, juce::Component* component)
{
    currentTheme = theme; // Store selected theme

    // Define colors for background, buttons, and text
    juce::Colour bgColor, btnColor, textColor;
    juce::Colour textBoxBgColor = getTextBoxBackgroundColor();  // Use helper function for consistency

    // **Switch Between Light, Dark, and Custom Themes**
    switch (theme)
    {
    case Light:
        bgColor = juce::Colours::white;       // White background
        btnColor = juce::Colours::lightgrey;  // Light grey buttons
        textColor = juce::Colours::black;     // Black text
        break;

    case Dark:
        bgColor = juce::Colours::black;       // Black background
        btnColor = juce::Colours::darkgrey;   // Dark grey buttons
        textColor = juce::Colours::white;     // White text for contrast
        textBoxBgColor = juce::Colours::darkgrey;  // Ensure text boxes are visible
        break;

    case Custom:
        bgColor = juce::Colours::lightpink;   // Light pink background
        btnColor = juce::Colours::hotpink;    // Hot pink buttons
        textColor = juce::Colours::purple;    // Purple text
        break;
    }

    // Apply colors to all UI components
    component->getLookAndFeel().setColour(juce::ResizableWindow::backgroundColourId, bgColor);
    component->getLookAndFeel().setColour(juce::TextButton::buttonColourId, btnColor);
    component->getLookAndFeel().setColour(juce::Label::textColourId, textColor);
    component->getLookAndFeel().setColour(juce::Slider::textBoxTextColourId, textColor);
    component->getLookAndFeel().setColour(juce::Slider::backgroundColourId, textBoxBgColor);  // Ensure text boxes are visible

    // Apply additional color settings
    component->getLookAndFeel().setColour(juce::Slider::thumbColourId, btnColor); // Match slider thumb to button color
    component->getLookAndFeel().setColour(juce::Label::backgroundColourId, bgColor); // Match label background to theme

    component->repaint(); // Refresh UI to apply new theme settings
}


// **Get Background Color Based on Current Theme**
juce::Colour ThemeManager::getBackgroundColor()
{
    return (currentTheme == Dark) ? juce::Colours::black :        // Dark mode: Black background
        (currentTheme == Custom) ? juce::Colours::lightpink :  // Custom mode: Light pink background
        juce::Colours::white; // Default (Light mode): White background
}

// **Get Button Color Based on Current Theme**
juce::Colour ThemeManager::getButtonColor()
{
    return (currentTheme == Dark) ? juce::Colours::darkgrey :     // Dark mode: Dark grey buttons
        (currentTheme == Custom) ? juce::Colours::hotpink :    // Custom mode: Hot pink buttons
        juce::Colours::lightgrey; // Default (Light mode): Light grey buttons
}

// **Get Text Color Based on Current Theme**
juce::Colour ThemeManager::getTextColor()
{
    return (currentTheme == Dark) ? juce::Colours::white :        // Dark mode: White text
        (currentTheme == Custom) ? juce::Colours::darkmagenta :// Custom mode: Dark magenta text
        juce::Colours::black; // Default (Light mode): Black text
}

// **Get Waveform Display Color Based on Current Theme**
juce::Colour ThemeManager::getWaveformColor()
{
    return (currentTheme == Dark) ? juce::Colours::cyan :         // Dark mode: Cyan waveform
        (currentTheme == Custom) ? juce::Colours::purple :     // Custom mode: Purple waveform
        juce::Colours::blue; // Default (Light mode): Blue waveform
}

// **Get Playlist Background Color Based on Current Theme**
juce::Colour ThemeManager::getPlaylistBackgroundColor()
{
    return (currentTheme == Dark) ? juce::Colour(30, 30, 30) :    // Dark mode: Dark grey background
        (currentTheme == Custom) ? juce::Colour(50, 50, 50) :  // Custom mode: Slightly lighter grey
        juce::Colours::white; // Default (Light mode): White background
}

// **Get Playlist Text Color Based on Current Theme**
juce::Colour ThemeManager::getPlaylistTextColor()
{
    return (currentTheme == Dark) ? juce::Colours::white :        // Dark mode: White text
        (currentTheme == Custom) ? juce::Colours::yellow :     // Custom mode: Yellow text
        juce::Colours::black; // Default (Light mode): Black text
}

