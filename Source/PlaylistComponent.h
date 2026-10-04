// Ensures this header file is only included once in a single compilation
#pragma once  

// Includes the JUCE framework for GUI and audio functionality
#include <JuceHeader.h>

// Includes DJAudioPlayer and DeckGUI for handling audio playback and controls
#include "DJAudioPlayer.h"
#include "DeckGUI.h"

// Defines the PlaylistComponent class, which manages the track list for the DJ system
class PlaylistComponent : public juce::Component,
    public juce::TableListBoxModel,  // Allows displaying tracks in a table format
    public juce::Button::Listener    // Enables handling button clicks within the playlist
{
public:
    // Fix constructor: Accepts two DJAudioPlayers and two DeckGUIs to manage playback
    PlaylistComponent(DJAudioPlayer* player1, DJAudioPlayer* player2, DeckGUI* deck1, DeckGUI* deck2);

    // Destructor: Cleans up resources when the object is destroyed
    ~PlaylistComponent() override;

    // Handles the graphical rendering of the component
    void paint(juce::Graphics&) override;

    // Called when the component is resized to adjust layout
    void resized() override;

    // Updates the UI theme (e.g., colors, fonts)
    void updateTheme();

    // Returns the total number of rows (tracks) in the playlist
    int getNumRows() override;

    // Paints the background of each row in the playlist
    void paintRowBackground(juce::Graphics&, int rowNumber, int width, int height, bool rowIsSelected) override;

    // Paints the text and content of each cell in the playlist table
    void paintCell(juce::Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

    // Refreshes UI components for cells (e.g., buttons for loading/deleting tracks)
    juce::Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate) override;

    // Handles button clicks inside the playlist (e.g., load, remove track buttons)
    void buttonClicked(juce::Button* button) override;

    // Opens a file browser to load tracks into the playlist
    void loadTracksFromFileBrowser();

private:
    // Table component for displaying the playlist
    juce::TableListBox tableComponent;

    // Stores the list of loaded track files
    std::vector<juce::File> trackFiles;

    // Pointers to the two DJ audio players for controlling playback
    DJAudioPlayer* player1;
    DJAudioPlayer* player2;

    // Add DeckGUI references to link with visual controls
    DeckGUI* deck1;
    DeckGUI* deck2;

    // Prevents copying and detects memory leaks in JUCE applications
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistComponent)
};
