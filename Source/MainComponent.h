// Ensures this header file is only included once during compilation
#pragma once  

// Includes the JUCE framework for audio and GUI functionality
#include <JuceHeader.h>

// Includes custom components for audio playback, GUI, and theme management
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include "PlaylistComponent.h"
#include "AudioRecorder.h"
#include "ThemeManager.h"  // ✅ Include ThemeManager for theme customization

// Defines the MainComponent class, which serves as the core application component
class MainComponent : public juce::AudioAppComponent
{
public:
    // Constructor: Initializes all components
    MainComponent();

    // Destructor: Cleans up resources when the object is destroyed
    ~MainComponent();

    // Prepares the audio system for playback by setting sample rate and buffer size
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

    // Retrieves the next block of audio data for processing
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

    // Releases resources when playback stops
    void releaseResources() override;

    // Handles the graphical rendering of the component
    void paint(juce::Graphics& g) override;

    // Called when the component is resized to adjust layout
    void resized() override;

private:
    // Manages audio formats for loading and decoding audio files
    juce::AudioFormatManager formatManager;

    // Cache for storing audio thumbnails (for waveform visualization)
    juce::AudioThumbnailCache thumbnailCache{ 20 };

    // DJ audio players for handling playback of two separate tracks
    DJAudioPlayer player1{ formatManager };
    DeckGUI deckGUI1{ &player1, formatManager, thumbnailCache };

    DJAudioPlayer player2{ formatManager };
    DeckGUI deckGUI2{ &player2, formatManager, thumbnailCache };

    // Playlist component for managing track selection and loading
    PlaylistComponent playlistComponent{ &player1, &player2, &deckGUI1, &deckGUI2 }; // ✅ Pass both DeckGUIs

    // Button to load tracks into the playlist
    juce::TextButton loadButton{ "Load Tracks" };

    // Mixer source for blending audio from both players
    juce::MixerAudioSource mixerSource;

    // Crossfader slider for smoothly transitioning between tracks
    juce::Slider crossFader{ juce::Slider::LinearHorizontal, juce::Slider::NoTextBox };

    // Function to handle crossfader adjustments
    void crossfaderChanged();

    // Audio recorder for recording mixes
    AudioRecorder recorder;

    // Audio source player to handle final audio output
    juce::AudioSourcePlayer audioSourcePlayer;

    // Button to start and stop recording
    juce::TextButton recordButton{ "Record" };

    // ✅ Dropdown menu for selecting different themes
    juce::ComboBox themeSelector;

    // Function to handle theme changes
    void themeChanged();

    // ✅ Prevents copying and detects memory leaks in JUCE applications
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
