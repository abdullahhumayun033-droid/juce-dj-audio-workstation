// Ensures this header file is only included once in a single compilation
#pragma once  

// Includes the JUCE framework for GUI and audio waveform visualization
#include <JuceHeader.h>

// Defines the WaveformDisplay class, which visualizes the waveform of an audio file
class WaveformDisplay : public juce::Component, public juce::ChangeListener
{
public:
    // Constructor: Initializes the waveform display with an audio format manager and thumbnail cache
    WaveformDisplay(juce::AudioFormatManager& formatManager, juce::AudioThumbnailCache& cache);

    // Destructor: Cleans up resources when the object is destroyed
    ~WaveformDisplay();

    // Handles the graphical rendering of the waveform
    void paint(juce::Graphics&) override;

    // Called when the component is resized to adjust layout
    void resized() override;

    // Callback function triggered when the audio thumbnail changes
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    // Loads an audio file into the waveform display
    void loadURL(juce::URL audioURL);

    // Sets the playback position relative to the total track length
    void setPositionRelative(double pos);

    // Vinyl Scratch Handling - Simulates a turntable-style scratching effect

    // Called when the mouse is pressed down (starts scratching)
    void mouseDown(const juce::MouseEvent& event) override;

    // Called when the mouse is dragged (modifies playback speed for scratching)
    void mouseDrag(const juce::MouseEvent& event) override;

    // Called when the mouse is released (ends scratching)
    void mouseUp(const juce::MouseEvent& event) override;

    // Function to update the waveform color dynamically
    void setWaveformColor(juce::Colour newColor);

    // Callback function to modify playback speed based on scratching
    std::function<void(double)> onScratch;

private:
    // Stores the waveform thumbnail for visualization
    juce::AudioThumbnail thumbnail;

    // Indicates whether a file has been loaded into the waveform display
    bool fileLoaded = false;

    // Stores the current playback position
    double position;

    //  Store last mouse position for scratching
    int lastMouseX = 0;

    // Indicates whether scratching is currently active
    bool isScratching = false;

    // Stores the current scratching speed
    double scratchSpeed = 1.0;

    // Label to display the playback speed while scratching
    juce::Label speedLabel;

    // Default waveform color, can be updated dynamically
    juce::Colour waveformColor = juce::Colours::blue;

    // Prevents copying and detects memory leaks in JUCE applications
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};
