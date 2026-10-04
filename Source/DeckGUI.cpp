#include "DeckGUI.h"
#include "ThemeManager.h"

// Constructor: Initializes the DeckGUI with a DJAudioPlayer, format manager, and thumbnail cache
DeckGUI::DeckGUI(DJAudioPlayer* _player, juce::AudioFormatManager& formatManager, juce::AudioThumbnailCache& cache)
    : player(_player), waveformDisplay(formatManager, cache)
{
    // Connect waveform scratching to DJAudioPlayer's scratch speed control
    waveformDisplay.onScratch = [this](double speed) {
        player->setScratchSpeed(speed);
    };

    // Make the beat visualizer visible in the UI
    addAndMakeVisible(beatVisualizer);

    // Connect DJAudioPlayer to BeatVisualizer to process audio for beat detection
    player->onAudioProcessed = [this](juce::AudioBuffer<float>& buffer) {
        beatVisualizer.processAudioBuffer(buffer);
    };

    // Configure buttons and sliders for user interaction
    configureButtons();
    configureSliders();

    // Add and display UI components
    addAndMakeVisible(waveformDisplay);
    addAndMakeVisible(loadButton);

    // Customize load button appearance
    loadButton.setColour(juce::TextButton::buttonColourId, juce::Colours::white);
    loadButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    loadButton.addListener(this);  // Set this class as the button's listener

    // Start a timer that runs every 200ms (used for periodic updates)
    startTimer(200);

    // Allow the component to receive keyboard input
    setWantsKeyboardFocus(true);
    addKeyListener(this);

    // Add labels for volume, speed, and position controls
    addAndMakeVisible(volLabel);
    addAndMakeVisible(speedLabel);
    addAndMakeVisible(posLabel);

    // Add sliders for volume, speed, and position
    addAndMakeVisible(volSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(posSlider);

    // Set text for labels
    volLabel.setText("Volume", juce::dontSendNotification);
    speedLabel.setText("Speed", juce::dontSendNotification);
    posLabel.setText("Position", juce::dontSendNotification);

    // Center align the labels for better UI consistency
    volLabel.setJustificationType(juce::Justification::centred);
    speedLabel.setJustificationType(juce::Justification::centred);
    posLabel.setJustificationType(juce::Justification::centred);

    // Set label text color to black for readability
    volLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    speedLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    posLabel.setColour(juce::Label::textColourId, juce::Colours::black);

    // Configure microphone button to toggle mic on/off
    micButton.onClick = [this]() {
        bool newState = !player->isMicEnabled();  // Get current mic state
        player->enableMic(newState);
        micButton.setButtonText(newState ? "Mic On" : "Mic Off");  // Update button text
    };

    // Make the mic button visible in the UI
    addAndMakeVisible(micButton);
}


// Destructor: Cleans up resources when the DeckGUI object is destroyed
DeckGUI::~DeckGUI()
{
    stopTimer();  // Stop the timer to prevent unnecessary callbacks after deletion
}

// Paints the DeckGUI component
void DeckGUI::paint(juce::Graphics& g)
{
    // Apply the current theme's background color to the entire component
    g.fillAll(ThemeManager::getBackgroundColor());

    // Debug message to check how often repainting occurs
    // If this appears too frequently, DeckGUI might be causing UI flickering
    DBG("🔴 DeckGUI repainting...");
}

// Called when the DeckGUI component is resized to reposition UI elements dynamically
void DeckGUI::resized()
{
    int totalWidth = getWidth();    // Get the total width of the component
    int totalHeight = getHeight();  // Get the total height of the component

    // Increase beat visualizer height from 30 to 200 for better visibility
    int beatVisualizerHeight = 200;

    // Divide the total height into 12 rows for better layout structuring
    int rowH = totalHeight / 12;

    // Divide the total width into 3 columns
    int colW = totalWidth / 3;

    // 🎚 **Waveform Display - Full Width, Positioned at the Top**
    waveformDisplay.setBounds(10, 10, totalWidth - 20, rowH * 2.5);

    // **Play, Pause, Stop, Load - Centered Below Waveform**
    int buttonSize = rowH * 1.5;   // Size for playback control buttons
    int buttonSpacing = 20;        // Space between buttons
    int buttonY = rowH * 3;        // Vertical position for playback buttons

    // Positioning playback buttons
    playButton.setBounds((totalWidth / 2) - (buttonSize * 2) - buttonSpacing, buttonY, buttonSize, buttonSize);
    pauseButton.setBounds((totalWidth / 2) - buttonSize, buttonY, buttonSize, buttonSize);
    stopButton.setBounds((totalWidth / 2) + buttonSpacing, buttonY, buttonSize, buttonSize);

    // **Load Button - Larger for Better Readability**
    loadButton.setBounds((totalWidth / 2) + buttonSize + buttonSpacing + 20, buttonY, buttonSize * 1.5, buttonSize);

    // **Sliders - Arranged in a Row**
    int sliderWidth = totalWidth / 8;  // Width of each slider
    int sliderHeight = rowH * 2;       // Height of sliders
    int sliderY = rowH * 5.5;          // Vertical position of sliders

    // Volume slider and label positioning
    volLabel.setBounds(totalWidth / 4 - sliderWidth / 2, sliderY - 20, sliderWidth, 20);
    volSlider.setBounds(totalWidth / 4 - sliderWidth / 2, sliderY, sliderWidth, sliderHeight);

    // Speed slider and label positioning
    speedLabel.setBounds(totalWidth / 2 - sliderWidth / 2, sliderY - 20, sliderWidth, 20);
    speedSlider.setBounds(totalWidth / 2 - sliderWidth / 2, sliderY, sliderWidth, sliderHeight);

    // Position slider and label positioning
    posLabel.setBounds((3 * totalWidth / 4) - sliderWidth / 2, sliderY - 20, sliderWidth, 20);
    posSlider.setBounds((3 * totalWidth / 4) - sliderWidth / 2, sliderY, sliderWidth, sliderHeight);

    // **Loop, Cue & Mic Controls - Bottom Row**
    int buttonW = totalWidth / 7;  // Width for loop, cue, and mic buttons
    int controlY = totalHeight - 130; // Vertical position of these buttons

    // Positioning loop, cue, and mic buttons
    loopInButton.setBounds(10, controlY, buttonW, 40);
    loopOutButton.setBounds(buttonW + 20, controlY, buttonW, 40);
    toggleLoopButton.setBounds((2 * buttonW) + 30, controlY, buttonW, 40);
    cueButton.setBounds((3 * buttonW) + 40, controlY, buttonW, 40);
    jumpCueButton.setBounds((4 * buttonW) + 50, controlY, buttonW, 40);
    micButton.setBounds((5 * buttonW) + 60, controlY, buttonW, 40);

    // **Beat Visualizer - Increased Size for Better Visibility**
    int visualizerY = controlY + buttonSize + 10; // Positioning below the control buttons
    beatVisualizer.setBounds(10, visualizerY, totalWidth - 20, beatVisualizerHeight);
}




// Handles button click events for various playback and control buttons
void DeckGUI::buttonClicked(juce::Button* button)
{
    // **Playback Controls**
    if (button == &playButton)
    {
        player->start();  // Start playback
    }
    else if (button == &pauseButton)
    {
        player->pause();  // Pause playback
    }
    else if (button == &stopButton)
    {
        player->stop();  // Stop playback
    }
    else if (button == &loadButton)  // **Load Audio File**
    {
        // Set file chooser to allow file selection
        auto fileChooserFlags = juce::FileBrowserComponent::canSelectFiles;

        // Launch file chooser asynchronously and handle selected file
        fChooser.launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser)
            {
                auto chosenFile = chooser.getResult();  // Get selected file

                if (chosenFile.existsAsFile())  // Ensure a valid file was selected
                {
                    player->loadURL(juce::URL{ chosenFile });  // Load file into player
                    waveformDisplay.loadURL(juce::URL{ chosenFile });  // Update waveform display
                }
            });
    }

    // **Loop Controls**
    if (button == &loopInButton)
    {
        // Set loop start point at the current playback position
        player->setLoopPoints(player->getPositionRelative(), player->getPositionRelative() + 0.1);
        DBG("Loop In Set");  // Debug message
    }
    else if (button == &loopOutButton)
    {
        // Set loop end point slightly ahead of the current position
        player->setLoopPoints(player->getPositionRelative(), player->getPositionRelative() + 0.2);
        DBG("Loop Out Set");  // Debug message
    }
    else if (button == &toggleLoopButton)
    {
        // Use getter function to check if looping is enabled, then toggle it
        bool looping = !player->isLoopEnabled();
        player->enableLoop(looping);
    }

    // **Cue Controls**
    else if (button == &cueButton)
    {
        player->setCuePoint();  // Set cue point at current position
    }
    else if (button == &jumpCueButton)
    {
        player->jumpToCuePoint();  // Jump to previously set cue point
    }
}



// Handles slider value changes for volume, speed, and position
void DeckGUI::sliderValueChanged(juce::Slider* slider)
{
    // **Volume Control**
    if (slider == &volSlider)
    {
        player->setGain(slider->getValue());  // Adjust the player's volume
    }

    // **Speed Control**
    if (slider == &speedSlider)
    {
        player->setSpeed(slider->getValue());  // Adjust the playback speed
    }

    // **Position Control**
    else if (slider == &posSlider)
    {
        player->setPositionRelative(slider->getValue());  // Seek to a new position in the track
    }
}

// **Drag & Drop Support**
// Determines whether the component should accept dragged files
bool DeckGUI::isInterestedInFileDrag(const juce::StringArray& files)
{
    return files.size() == 1;  // Accept only a single file at a time
}


// **Drag & Drop File Handling**
void DeckGUI::filesDropped(const juce::StringArray& files, int x, int y)
{
    // Ensure only one file is accepted
    if (files.size() == 1)
    {
        juce::File file(files[0]);  // Convert file path to JUCE file object

        if (file.existsAsFile())  // Check if file is valid
        {
            player->loadURL(juce::URL{ file });  // Load audio file into player
            waveformDisplay.loadURL(juce::URL{ file });  // Update waveform display
        }
    }
}

// **Keyboard Shortcuts for Playback Control**
bool DeckGUI::keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent)
{
    // **Play or Resume** when 'p' is pressed
    if (key.getTextCharacter() == 'p')
    {
        player->start();
        return true;
    }

    // **Pause** when the Spacebar is pressed
    else if (key.getTextCharacter() == ' ') // Spacebar for Pause
    {
        player->pause();
        return true;
    }

    // **Stop** when 's' is pressed
    else if (key.getTextCharacter() == 's')
    {
        player->stop();
        return true;
    }

    // **Trigger File Load** when 'l' is pressed
    else if (key.getTextCharacter() == 'l')
    {
        loadButton.triggerClick();  // Simulate a button click on the Load button
        return true;
    }

    return false; // Return false to allow other components to handle unprocessed keys
}



// **Timer Callback for Waveform Position Updates**
void DeckGUI::timerCallback()
{
    if (player)  // Ensure the player exists before accessing it
    {
        // Update the waveform display's position based on the player's current position
        waveformDisplay.setPositionRelative(player->getPositionRelative());
    }
}

// **Configure Buttons with Images and Event Listeners**
void DeckGUI::configureButtons()
{
    // Load images for play, pause, and stop buttons from binary resources
    auto playImage = juce::ImageCache::getFromMemory(BinaryData::play_png, BinaryData::play_pngSize);
    auto pauseImage = juce::ImageCache::getFromMemory(BinaryData::pause_png, BinaryData::pause_pngSize);
    auto stopImage = juce::ImageCache::getFromMemory(BinaryData::stop_png, BinaryData::stop_pngSize);

    // **Configure Stop Button with Image**
    stopButton.setImages(true, true, true,
        stopImage, 0.5f, juce::Colours::transparentBlack,  // Normal state
        stopImage, 1.0f, juce::Colours::transparentBlack,  // Hover state
        stopImage, 0.5f, juce::Colours::transparentBlack); // Pressed state

    // **Configure Play Button with Image**
    playButton.setImages(true, true, true,
        playImage, 0.5f, juce::Colours::transparentBlack,  // Normal state
        playImage, 1.0f, juce::Colours::transparentBlack,  // Hover state
        playImage, 0.5f, juce::Colours::transparentBlack); // Pressed state

    // **Configure Pause Button with Image**
    pauseButton.setImages(true, true, true,
        pauseImage, 0.5f, juce::Colours::transparentBlack,  // Normal state
        pauseImage, 1.0f, juce::Colours::transparentBlack,  // Hover state
        pauseImage, 0.5f, juce::Colours::transparentBlack); // Pressed state

    // Add playback buttons to the UI
    addAndMakeVisible(playButton);
    addAndMakeVisible(pauseButton);
    addAndMakeVisible(stopButton);

    // Register this class as a listener for button click events
    playButton.addListener(this);
    pauseButton.addListener(this);
    stopButton.addListener(this);

    // **Add Loop & Cue Buttons**
    addAndMakeVisible(loopInButton);
    addAndMakeVisible(loopOutButton);
    addAndMakeVisible(toggleLoopButton);
    addAndMakeVisible(cueButton);
    addAndMakeVisible(jumpCueButton);

    // Register event listeners for loop and cue buttons
    loopInButton.addListener(this);
    loopOutButton.addListener(this);
    toggleLoopButton.addListener(this);
    cueButton.addListener(this);
    jumpCueButton.addListener(this);
}


// 🎚 **Configure Sliders for Volume, Speed, and Position**
void DeckGUI::configureSliders()
{
    // Set the volume slider range (0 = mute, 1 = full volume)
    volSlider.setRange(0.0, 1.0);

    // Incorrect range for speed slider (100 is too high)
    // Adjusted speed range to a more realistic DJ-style range (0.5x to 2.0x)
    speedSlider.setRange(0.5, 2.0);

    // Set the default playback speed to 1.0 (normal speed)
    speedSlider.setValue(1.0, juce::dontSendNotification);

    // Set position slider range (0.0 = start of track, 1.0 = end of track)
    posSlider.setRange(0.0, 1.0);

    // Use Rotary Sliders for a more DJ-like experience
    volSlider.setSliderStyle(juce::Slider::Rotary);
    speedSlider.setSliderStyle(juce::Slider::Rotary);
    posSlider.setSliderStyle(juce::Slider::Rotary);

    // Ensure sliders have visible colors
    volSlider.setColour(juce::Slider::trackColourId, juce::Colours::white);
    speedSlider.setColour(juce::Slider::trackColourId, juce::Colours::white);
    posSlider.setColour(juce::Slider::trackColourId, juce::Colours::white);

    // Different thumb colors for better distinction
    volSlider.setColour(juce::Slider::thumbColourId, juce::Colours::red);
    speedSlider.setColour(juce::Slider::thumbColourId, juce::Colours::blue);
    posSlider.setColour(juce::Slider::thumbColourId, juce::Colours::green);

    // Ensure sliders are added to the GUI for visibility
    addAndMakeVisible(volSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(posSlider);

    // Register this class as a listener for slider value changes
    volSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);
}




// **File Drag & Drop UI Feedback**

// Called when a file is dragged into the DeckGUI component
void DeckGUI::fileDragEnter(const juce::StringArray& files, int x, int y)
{
    shouldDrawDragHighlight = true; // Highlight UI to indicate drag state
    repaint(); // Trigger a UI update to reflect the change
}

// Called when the dragged file leaves the component without being dropped
void DeckGUI::fileDragExit(const juce::StringArray& files)
{
    shouldDrawDragHighlight = false; // Remove drag highlight
    repaint(); // Trigger a UI update to reflect the change
}

// **Update the Waveform Display with a New Audio File**
void DeckGUI::updateWaveform(juce::URL audioURL)
{
    waveformDisplay.loadURL(audioURL); // Load the new audio file into the waveform display
}

// **Update Theme Colors for UI Elements**
void DeckGUI::updateTheme()
{
    // Update label text color based on the current theme
    volLabel.setColour(juce::Label::textColourId, ThemeManager::getTextColor());
    speedLabel.setColour(juce::Label::textColourId, ThemeManager::getTextColor());
    posLabel.setColour(juce::Label::textColourId, ThemeManager::getTextColor());

    // Update volume slider text and background color
    volSlider.setColour(juce::Slider::textBoxTextColourId, ThemeManager::getTextColor());
    volSlider.setColour(juce::Slider::backgroundColourId, ThemeManager::getTextBoxBackgroundColor());

    // Update speed slider text and background color
    speedSlider.setColour(juce::Slider::textBoxTextColourId, ThemeManager::getTextColor());
    speedSlider.setColour(juce::Slider::backgroundColourId, ThemeManager::getTextBoxBackgroundColor());

    // Update position slider text and background color
    posSlider.setColour(juce::Slider::textBoxTextColourId, ThemeManager::getTextColor());
    posSlider.setColour(juce::Slider::backgroundColourId, ThemeManager::getTextBoxBackgroundColor());

    // Update the waveform color based on the current theme
    waveformDisplay.setWaveformColor(ThemeManager::getWaveformColor());

    repaint(); // Ensure UI refreshes to apply new colors
}