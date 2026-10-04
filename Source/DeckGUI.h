// Ensures this header file is included only once in a single compilation
#pragma once  

// Includes the JUCE framework for GUI and audio functionality
#include <JuceHeader.h>

// Includes necessary custom components for audio playback and visualization
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"
#include "BeatVisualizer.h"  //  Includes BeatVisualizer for beat detection visualization

// Defines the DeckGUI class, which handles the user interface for audio playback
class DeckGUI : public juce::Component,
    public juce::Button::Listener,       // Allows DeckGUI to respond to button clicks
    public juce::Slider::Listener,       // Allows DeckGUI to respond to slider changes
    public juce::FileDragAndDropTarget,  // Enables drag-and-drop file support
    public juce::Timer,                  // Uses a timer to update UI elements periodically
    public juce::KeyListener             // Enables keyboard shortcuts for controls
{
public:
    // Constructor: Initializes the GUI with a DJAudioPlayer, format manager, and cache
    DeckGUI(DJAudioPlayer* player, juce::AudioFormatManager& formatManager, juce::AudioThumbnailCache& cache);

    // Destructor: Cleans up resources when the object is destroyed
    ~DeckGUI();

    // Periodic function called by the timer to update UI elements
    void timerCallback() override;

    // Handles the graphical rendering of the component
    void paint(juce::Graphics&) override;

    // Called when the component is resized, used to adjust layout
    void resized() override;

    // Handles button click events
    void buttonClicked(juce::Button*) override;

    // Handles slider value changes
    void sliderValueChanged(juce::Slider* slider) override;

    // Determines whether the component should accept a dragged file
    bool isInterestedInFileDrag(const juce::StringArray& files) override;

    // Called when files are dropped onto the component
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    // Handles keyboard input events for shortcuts
    bool keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) override;

    // Flag to indicate whether a file drag highlight should be drawn
    bool shouldDrawDragHighlight = false; // Add a flag

    // Called when a file is dragged into the component
    void fileDragEnter(const juce::StringArray& files, int x, int y) override;

    // Called when a dragged file leaves the component without being dropped
    void fileDragExit(const juce::StringArray& files) override;

    // Updates the waveform display with a new audio file
    void updateWaveform(juce::URL audioURL); // Add this function

    // Updates the theme of the interface (e.g., colors, styles)
    void updateTheme(); // Add this function

    // Waveform display for visualizing audio playback
    WaveformDisplay waveformDisplay;

private:
    // Visualizer for displaying detected beats in the audio
    BeatVisualizer beatVisualizer;  //  Add BeatVisualizer

    // Button for toggling microphone input on or off
    juce::TextButton micButton{ "Mic On/Off" };

    // Playback control buttons
    juce::ImageButton playButton;
    juce::ImageButton pauseButton;
    juce::ImageButton stopButton;

    // Button to load an audio file into the player
    juce::TextButton loadButton{ "LOAD" };

    // Rotary sliders for volume, speed, and position control
    juce::Slider volSlider{ juce::Slider::Rotary, juce::Slider::TextBoxBelow };
    juce::Slider speedSlider{ juce::Slider::Rotary, juce::Slider::TextBoxBelow };
    juce::Slider posSlider{ juce::Slider::Rotary, juce::Slider::TextBoxBelow };

    // File chooser for selecting audio files
    juce::FileChooser fChooser{ "Select a file..." };

    // Pointer to the DJAudioPlayer instance controlling playback
    DJAudioPlayer* player;

    // Loop & Cue Buttons for DJ-style playback control
    juce::TextButton loopInButton{ "Loop In" };       // Sets loop start point
    juce::TextButton loopOutButton{ "Loop Out" };     // Sets loop end point
    juce::TextButton toggleLoopButton{ "Enable Loop" }; // Toggles looping on/off
    juce::TextButton cueButton{ "Set Cue" };          // Sets a cue point
    juce::TextButton jumpCueButton{ "Jump to Cue" };  // Jumps to the saved cue point

    // Labels for volume, speed, and position controls
    juce::Label volLabel;
    juce::Label speedLabel;
    juce::Label posLabel;

    // Configures button properties and listeners
    void configureButtons();

    // Configures slider properties and listeners
    void configureSliders();

    // Prevents copying and detects memory leaks in JUCE applications
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeckGUI)
};
