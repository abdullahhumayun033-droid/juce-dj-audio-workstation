#include "PlaylistComponent.h"
#include "ThemeManager.h"

// **Playlist Component Constructor (Manages Track List & Deck Loading)**
PlaylistComponent::PlaylistComponent(DJAudioPlayer* player1, DJAudioPlayer* player2, DeckGUI* deck1, DeckGUI* deck2)
    : player1(player1), player2(player2), deck1(deck1), deck2(deck2) // Assign DeckGUI pointers
{
    addAndMakeVisible(tableComponent); // Ensure the table is visible

    // **Add Columns to the Playlist Table**
    tableComponent.getHeader().addColumn("Track Title", 1, 300);  // Track name column
    tableComponent.getHeader().addColumn("Deck 1", 2, 100);       // Load to Deck 1 button
    tableComponent.getHeader().addColumn("Deck 2", 3, 100);       // Load to Deck 2 button
    tableComponent.getHeader().addColumn("Remove", 4, 100);       // Remove track button

    // **Set Playlist Table Properties**
    tableComponent.setModel(this); // Use this class as the data source for the table
    tableComponent.setColour(juce::TableListBox::backgroundColourId, juce::Colours::white); // Set table background color
}



// ❌ **Destructor: Cleans Up Resources (Currently Empty, But Ready for Expansion)**
PlaylistComponent::~PlaylistComponent() {}

// **Get the Number of Tracks in the Playlist**
int PlaylistComponent::getNumRows()
{
    return trackFiles.size(); // Return the total number of tracks
}

// **Paint Row Background Based on Selection**
void PlaylistComponent::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    // Highlight the selected row, otherwise keep the background white
    g.fillAll(rowIsSelected ? juce::Colours::lightgrey : juce::Colours::white);
}

// **Paint Individual Playlist Cells (Track Title, Buttons)**
void PlaylistComponent::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    g.setColour(ThemeManager::getPlaylistTextColor());  // Use theme-based text color

    // If it's the first column, display the track title
    if (columnId == 1)
    {
        g.drawText(trackFiles[rowNumber].getFileName(),  // Get the track's filename
            2, 0, width - 4, height,  // Define text boundaries
            juce::Justification::centredLeft,  // Align text to the left
            true);  // Enable text clipping if needed
    }
}


// **Refresh UI Components for Each Cell in the Playlist Table**
juce::Component* PlaylistComponent::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate)
{
    // Try casting the existing component to a TextButton
    auto* btn = dynamic_cast<juce::TextButton*>(existingComponentToUpdate);

    // If the column is for Deck 1, Deck 2, or Remove buttons, create/update a button
    if (columnId == 2 || columnId == 3 || columnId == 4)
    {
        // If no button exists, create a new one
        if (!btn)
        {
            btn = new juce::TextButton(); // Create new button instance
            btn->setComponentID(std::to_string(rowNumber) + "-" + std::to_string(columnId)); // Assign unique ID for tracking
            btn->addListener(this); // Register this class as a listener for button clicks
            btn->setColour(juce::TextButton::buttonColourId, juce::Colours::white); // Set button background color
            btn->setColour(juce::TextButton::textColourOffId, juce::Colours::black); // Set button text color
        }

        // 🏷 **Set Button Labels Based on Column**
        if (columnId == 2)
            btn->setButtonText("Deck 1"); // Button for loading track into Deck 1
        else if (columnId == 3)
            btn->setButtonText("Deck 2"); // Button for loading track into Deck 2
        else if (columnId == 4)
            btn->setButtonText("Remove"); // Button for removing track from the playlist

        existingComponentToUpdate = btn; // Assign updated button to the cell
    }

    return existingComponentToUpdate; // Return the updated component
}


// **Handle Button Click Events in the Playlist Table**
void PlaylistComponent::buttonClicked(juce::Button* button)
{
    // Retrieve the button's unique component ID (format: "rowNumber-columnId")
    juce::String componentID = button->getComponentID();

    // Extract row number (track index) and column ID (button type)
    int dashPos = componentID.indexOfChar('-'); // Find position of '-'
    int id = componentID.substring(0, dashPos).getIntValue(); // Extract row number
    int columnId = componentID.substring(dashPos + 1).getIntValue(); // Extract column ID

    // Ensure valid track index before proceeding
    if (id >= 0 && id < trackFiles.size())
    {
        juce::URL fileURL = juce::URL(trackFiles[id]); // Get track URL

        // ▶ **Load Track into Deck 1**
        if (columnId == 2)
        {
            player1->loadURL(fileURL); // Load the track into Deck 1
            player1->start(); // Start playback immediately
            deck1->updateWaveform(fileURL); // Update waveform display for Deck 1
        }
        // ▶ **Load Track into Deck 2**
        else if (columnId == 3)
        {
            player2->loadURL(fileURL); // Load the track into Deck 2
            player2->start(); // Start playback immediately
            deck2->updateWaveform(fileURL); // Update waveform display for Deck 2
        }
        // ❌ **Remove Track from Playlist**
        else if (columnId == 4)
        {
            trackFiles.erase(trackFiles.begin() + id); // Remove track from the list
            tableComponent.updateContent(); // Refresh the table to reflect changes
            repaint(); // Redraw the component
        }
    }
}



// **Load Tracks from File Browser and Add to Playlist**
void PlaylistComponent::loadTracksFromFileBrowser()
{
    DBG("✅ Opening FileChooser..."); // Debug output to indicate file browser is opening

    static std::unique_ptr<juce::FileChooser> chooser; // Use a static pointer to prevent premature destruction
    chooser = std::make_unique<juce::FileChooser>(
        "Select Audio Files",
        juce::File(),
        "*.wav;*.mp3;*.ogg"); // Allow multiple audio file types

    // Launch file chooser asynchronously to avoid blocking the UI
    chooser->launchAsync(juce::FileBrowserComponent::canSelectMultipleItems,
        [this](const juce::FileChooser& fc)
        {
            auto files = fc.getResults(); // Get selected files

            if (files.isEmpty()) // Handle case where no files are selected
            {
                DBG("❌ No files selected!");
                return;
            }

            // **Loop Through Selected Files and Add Them to Playlist**
            for (const auto& file : files)
            {
                DBG("✅ File added to playlist: " + file.getFullPathName()); // Debug output
                trackFiles.push_back(file); // Store file in playlist
            }

            tableComponent.updateContent(); // Refresh the table to display new tracks
            repaint();  // Ensure UI refreshes properly
        });
}

// **Paint Playlist Background Based on Theme**
void PlaylistComponent::paint(juce::Graphics& g)
{
    g.fillAll(ThemeManager::getPlaylistBackgroundColor());  // Set dynamic background color from theme
}

// **Resize Playlist Table to Fit Component**
void PlaylistComponent::resized()
{
    tableComponent.setBounds(0, 0, getWidth(), getHeight());  // Ensure table fills the entire component
}

// **Update Theme When Theme Changes**
void PlaylistComponent::updateTheme()
{
    repaint(); // Force UI refresh to apply new theme settings
}


