#include "WaveformDisplay.h"
#include "ThemeManager.h"
using namespace juce;

// **Waveform Display Constructor (Handles Audio Visualization)**
WaveformDisplay::WaveformDisplay(AudioFormatManager& formatManagerToUse,
    AudioThumbnailCache& cacheToUse)
    : thumbnail(1000, formatManagerToUse, cacheToUse), // Initialize audio thumbnail
    fileLoaded(false), // Track whether a file is loaded
    position(0), // Ensure proper initialization order
    scratchSpeed(1.0)  // Initialize scratch speed to normal (1.0x)
{
    thumbnail.addChangeListener(this); // Register as a listener for thumbnail changes

    // **Setup Speed Label for Vinyl Scratch Effect**
    addAndMakeVisible(speedLabel); // Ensure speed label is visible when needed
    speedLabel.setFont(juce::Font(15.0f, juce::Font::bold)); // Set label font
    speedLabel.setColour(juce::Label::textColourId, juce::Colours::white); // White text
    speedLabel.setJustificationType(juce::Justification::centred); // Center-align text
    speedLabel.setVisible(false); // Initially hidden, shown only during scratching
}

// ❌ **Destructor (Currently Empty, But Ready for Expansion)**
WaveformDisplay::~WaveformDisplay() {}

// **Update Waveform Position Relative to Track Length**
void WaveformDisplay::setPositionRelative(double pos)
{
    if (pos != position) // Only update if the position has changed
    {
        position = pos;
        repaint(); // Redraw waveform to reflect new position
    }

    if (!fileLoaded) // If no file is loaded, force a repaint
    {
        repaint();
    }
}


// **Render the Waveform Display**
void WaveformDisplay::paint(juce::Graphics& g)
{
    g.fillAll(ThemeManager::getBackgroundColor()); // Apply themed background color

    g.setColour(waveformColor); // Use the updated waveform color from theme settings

    // **Draw the Audio Waveform**
    thumbnail.drawChannel(g, getLocalBounds(),  // Draw the waveform in the full component area
        0, thumbnail.getTotalLength(),  // Display full track length
        0, 1.0f);  // Channel index 0, full amplitude scaling

    g.drawRect(getLocalBounds(), 2); // Add a thick border around the waveform

    // **If a File is Loaded, Draw the Playhead and Waveform**
    if (fileLoaded)
    {
        g.setColour(juce::Colours::blue); // Waveform color
        thumbnail.drawChannel(g, getLocalBounds(),  // Redraw the waveform in blue
            0, thumbnail.getTotalLength(),
            0, 1.0f);

        // **Draw the Playhead Line**
        g.setColour(juce::Colours::red);
        int playheadX = static_cast<int>(position * getWidth()); // Calculate playhead position
        g.drawLine(playheadX, 0, playheadX, getHeight(), 2.0f); // Draw vertical red playhead
    }
    else
    {
        // ❌ **Display Placeholder Text When No File is Loaded**
        g.setFont(20.0f); // Set font size for placeholder text
        g.setColour(juce::Colours::grey); // Use grey color for placeholder text
        g.drawText("File not loaded...", getLocalBounds(),  // Centered placeholder text
            Justification::centred, true);
    }
}



// **Handle Component Resizing (Currently Empty, But Ready for Expansion)**
void WaveformDisplay::resized() {}

// **Load an Audio File into the Waveform Display**
void WaveformDisplay::loadURL(juce::URL audioURL)
{
    juce::File audioFile = audioURL.getLocalFile(); // Convert URL to a local file

    // ❌ **Check If File Exists Before Proceeding**
    if (!audioFile.existsAsFile())
    {
        DBG("WaveformDisplay::loadURL - ERROR: File does not exist."); // Debug log for missing file
        return;
    }

    // **Attempt to Load the File into the Thumbnail**
    if (thumbnail.setSource(new juce::FileInputSource(audioFile)))
    {
        fileLoaded = true;  // Mark file as successfully loaded
        repaint();  // Ensure waveform updates visually
    }
    else
    {
        DBG("WaveformDisplay::loadURL - ERROR: Failed to set source."); // Debug log for load failure
    }
}

// **Repaint When Waveform Data Changes**
void WaveformDisplay::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &thumbnail) repaint(); // Redraw waveform when thumbnail updates
}

// **Handle Mouse Click (Start Scratching)**
void WaveformDisplay::mouseDown(const juce::MouseEvent& event)
{
    isScratching = true; // Enable scratching mode
    lastMouseX = event.x;  // Store the initial mouse X position for tracking movement
    speedLabel.setVisible(true);  // Show speed label to indicate scratching effect
}

// **Handle Mouse Drag (Simulate Scratching Effect)**
void WaveformDisplay::mouseDrag(const juce::MouseEvent& event)
{
    if (isScratching) // Only process if scratching is enabled
    {
        int deltaX = event.x - lastMouseX;  // Calculate horizontal mouse movement distance

        double sensitivity = 0.02;  // Adjust scratching sensitivity (higher = stronger scratch effect)
        scratchSpeed = juce::jlimit(0.2, 3.0, 1.0 + (deltaX * sensitivity));  // Limit scratching speed variation

        if (onScratch) // If a scratch function is linked, send updated speed
            onScratch(scratchSpeed);

        lastMouseX = event.x;  // Store current position for next frame

        // **Update Speed Label Dynamically**
        speedLabel.setText(juce::String(scratchSpeed, 2) + "x", juce::dontSendNotification); // Display speed multiplier
        speedLabel.setBounds(getWidth() / 2 - 25, getHeight() / 2 - 10, 50, 20); // Center label on waveform
        repaint(); // Redraw UI to reflect changes
    }
}

// **Handle Mouse Release (Stop Scratching)**
void WaveformDisplay::mouseUp(const juce::MouseEvent& event)
{
    isScratching = false; // Disable scratching mode

    // Only reset speed if it was changed by scratching
    if (scratchSpeed != 1.0)
    {
        scratchSpeed = 1.0;
        if (onScratch)
            DBG("🔄 Resetting scratch speed to 1.0 in mouseUp()");
            onScratch(1.0);  // Reset speed to normal playback speed
    }

    speedLabel.setVisible(false);  // Hide speed label when scratching stops
}

// **Change Waveform Color Dynamically**
void WaveformDisplay::setWaveformColor(juce::Colour newColor)
{
    waveformColor = newColor; // Store new waveform color
    repaint(); // Refresh UI to apply the new color
}

