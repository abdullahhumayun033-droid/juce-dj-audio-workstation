#include "MainComponent.h"
#include "DeckGUI.h"  // Include DeckGUI for DJ interface

// **Main Component Constructor (Initializes UI & Audio)**
MainComponent::MainComponent()
    : playlistComponent(&player1, &player2, &deckGUI1, &deckGUI2) // Pass both DeckGUIs to PlaylistComponent
{
    setSize(800, 600); // Set initial window size

    // **Handle Microphone Permissions (Required for Recording)**
    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
        && !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
            [&](bool granted) { if (granted) setAudioChannels(2, 2); }); // Request permission before enabling audio
    }
    else
    {
        setAudioChannels(2, 2); // Enable stereo audio channels (2 input, 2 output)
    }

    formatManager.registerBasicFormats(); // Register common audio formats (MP3, WAV, etc.)

    // **Load Button for Adding Tracks**
    loadButton.onClick = [this] {
        DBG("✅ Load Tracks button clicked!"); // Debug message for click event
        playlistComponent.loadTracksFromFileBrowser(); // Open file browser to load tracks
    };

    // Add UI Components to the Window
    addAndMakeVisible(deckGUI1);
    addAndMakeVisible(deckGUI2);
    addAndMakeVisible(playlistComponent);
    addAndMakeVisible(loadButton);

    loadButton.setClickingTogglesState(false); // Ensure button doesn't stay toggled
    loadButton.setTriggeredOnMouseDown(true); // Trigger click event on mouse down

    // **Theme Selector (Dropdown for Light/Dark Mode)**
    themeSelector.addItem("Light", 1);
    themeSelector.addItem("Dark", 2);
    themeSelector.addItem("Custom", 3);
    themeSelector.onChange = [this] { themeChanged(); }; // Apply new theme when selected
    addAndMakeVisible(themeSelector);

    // **Crossfader (Blends Two Tracks)**
    crossFader.setSliderStyle(juce::Slider::LinearHorizontal); // Keep it horizontal
    crossFader.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0); // Remove text box for cleaner UI
    crossFader.setRange(0.0, 1.0, 0.01); // Ensure smooth transitions from left (0.0) to right (1.0)
    crossFader.setValue(0.5); // Start in the middle (equal mix of both decks)

    //  Apply Custom Colors
    crossFader.setColour(juce::Slider::trackColourId, juce::Colours::darkgrey); // Dark grey track
    crossFader.setColour(juce::Slider::thumbColourId, juce::Colours::red); // Red thumb (knob)

    // Optional: Add hover effect (turns yellow when moved)
    crossFader.onValueChange = [this] {
        crossFader.setColour(juce::Slider::thumbColourId, juce::Colours::yellow);
        crossfaderChanged(); // Adjust audio mix when crossfader is moved
    };
    addAndMakeVisible(crossFader);

    // **Recording Button (Start/Stop Audio Recording)**
    recordButton.onClick = [this] {
        if (recorder.isRecording()) {
            recorder.stopRecording(); // Stop recording
            recordButton.setButtonText("Record"); // Reset button text
        }
        else {
            // Save recorded mix as "DJ_Mix.wav" in the user's Documents folder
            juce::File file(juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                .getChildFile("DJ_Mix.wav"));
            recorder.startRecording(file); // Start recording
            recordButton.setButtonText("Stop Recording"); // Update button text
        }
    };

    addAndMakeVisible(recordButton); // Ensure record button is visible
}




// ❌ **Destructor: Cleans Up Audio Resources**
MainComponent::~MainComponent()
{
    shutdownAudio(); // Properly shut down audio when the component is destroyed
}

// **Prepare Audio System for Playback**
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // Log sample rate for debugging
    DBG("MainComponent::prepareToPlay - SampleRate: " + juce::String(sampleRate));

    // **Prepare Both Decks for Playback**
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);

    // Ensure default playback speed is set to normal (1.0x)
    player1.setSpeed(1.0);
    player2.setSpeed(1.0);

    // **Prepare the Audio Recorder**
    recorder.prepareToPlay(samplesPerBlockExpected, sampleRate);

    // **Ensure Mixer Gets Audio from Both DJ Decks**
    mixerSource.addInputSource(&player1, false); // Add Deck 1 to the mixer
    mixerSource.addInputSource(&player2, false); // Add Deck 2 to the mixer

    // **Route Mixer Output to the Audio System**
    deviceManager.addAudioCallback(&audioSourcePlayer); // Connect mixer to audio output
    audioSourcePlayer.setSource(&mixerSource); // Ensure output is properly handled

    // **Ensure Recorder Captures Final Mixed Output**
    mixerSource.addInputSource(&recorder, false);  // Connect the recorder to the mixer
}



// **Fetch the Next Block of Mixed Audio for Playback**
void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Get the mixed output from both decks
    mixerSource.getNextAudioBlock(bufferToFill);

    // **Record the Same Audio Being Played**
    if (recorder.isRecording()) {
        recorder.getNextAudioBlock(bufferToFill); // Capture the mixed output while playing
    }
}

// ❌ **Release Audio Resources When Not Needed**
void MainComponent::releaseResources()
{
    player1.releaseResources();  // Release Deck 1 resources
    player2.releaseResources();  // Release Deck 2 resources
    mixerSource.releaseResources();  // Release mixer resources
    recorder.releaseResources();  // Free recorder buffer

    // 🚫 **Disconnect Audio Output**
    audioSourcePlayer.setSource(nullptr); // Remove the audio source to prevent crashes
    deviceManager.removeAudioCallback(&audioSourcePlayer); // Unregister audio callback
}


// **Paint the Background**
void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::white); // Set background color to white
}

// **Arrange UI Components When Resized**
void MainComponent::resized()
{
    // **Position Deck GUIs (Each Deck Takes Half of the Top Section)**
    deckGUI1.setBounds(0, 0, getWidth() / 2, getHeight() / 2); // Deck 1 (Left)
    deckGUI2.setBounds(getWidth() / 2, 0, getWidth() / 2, getHeight() / 2); // Deck 2 (Right)

    // **Playlist Component Below Both Decks**
    playlistComponent.setBounds(0, getHeight() / 2, getWidth(), getHeight() / 2);

    // **Move Load Button to the Bottom**
    loadButton.setBounds(10, getHeight() - 50, getWidth() - 20, 40);

    // **Crossfader Positioned Above Decks**
    crossFader.setBounds(getWidth() / 2 - 100, getHeight() - 590, 200, 70);

    // **Record Button Positioned Above the Crossfader**
    recordButton.setBounds(getWidth() / 2 - 100, getHeight() - 620, 200, 40);

    // **Theme Selector Dropdown Positioned at the Top-Left Corner**
    themeSelector.setBounds(10, 10, 150, 30);
}

// **Handle Crossfader Movement to Adjust Deck Volumes**
void MainComponent::crossfaderChanged()
{
    float crossfadeValue = crossFader.getValue(); // Get current crossfade position (0.0 = left, 1.0 = right)

    float volume1 = 1.0f - crossfadeValue; // Player 1 fades out as slider moves right
    float volume2 = crossfadeValue;        // Player 2 fades in as slider moves right

    player1.setGain(volume1); // Apply volume changes to Player 1
    player2.setGain(volume2); // Apply volume changes to Player 2

    // Debug log for crossfade values (useful for troubleshooting)
    DBG("Crossfade Value: " + juce::String(crossfadeValue) +
        " | Volume1: " + juce::String(volume1) +
        " | Volume2: " + juce::String(volume2));
}




// **Apply Selected Theme to the Application**
void MainComponent::themeChanged()
{
    // Determine which theme is selected from the dropdown
    ThemeManager::ThemeType selectedTheme =
        (themeSelector.getSelectedId() == 2) ? ThemeManager::Dark :
        (themeSelector.getSelectedId() == 3) ? ThemeManager::Custom :
        ThemeManager::Light; // Default to Light theme if no match

    // Apply the selected theme to the main component
    ThemeManager::applyTheme(selectedTheme, this);

    // **Update UI Elements to Reflect Theme Changes**
    deckGUI1.updateTheme(); // Update Deck 1 UI
    deckGUI2.updateTheme(); // Update Deck 2 UI
    playlistComponent.updateTheme(); // Ensure Playlist updates to match the theme
}


