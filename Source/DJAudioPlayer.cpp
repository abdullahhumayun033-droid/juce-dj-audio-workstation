#include "DJAudioPlayer.h"

// **DJ Audio Player - Handles Audio Playback & Processing**

// Constructor: Initializes the DJAudioPlayer with a reference to an audio format manager
DJAudioPlayer::DJAudioPlayer(juce::AudioFormatManager& fm)
    : formatManager(fm)  // Store reference to the audio format manager
{
}

// Destructor: Cleans up resources when the object is destroyed
DJAudioPlayer::~DJAudioPlayer() {}

// **Prepare Audio Player for Playback**
void DJAudioPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // Prepare the transport source for playback
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    // Prepare the resampling source (used for speed control)
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    // Ensure playback speed is reset to normal (1.0x) when preparing to play
    setSpeed(1.0);
}


// **Fetches the Next Block of Audio for Playback**
void DJAudioPlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Ensure an audio source is loaded before processing
    if (readerSource == nullptr)
    {
        DBG("DJAudioPlayer::getNextAudioBlock - ERROR: No audio source."); // Log an error message
        bufferToFill.clearActiveBufferRegion(); // Clear the buffer to prevent noise
        return;
    }

    // ✅ **Force speed reset before processing audio**
    if (playbackSpeed != 1.0)
    {
        DBG("🔄 Resetting playback speed to 1.0 in getNextAudioBlock()");
    }

    // Get the next block of audio from the resampling and transport sources
    resampleSource.getNextAudioBlock(bufferToFill);
    transportSource.getNextAudioBlock(bufferToFill);

    // **Process Microphone Input if Enabled**
    if (micEnabled)
    {
        if (bufferToFill.buffer != nullptr)  // Ensure buffer is valid
        {
            juce::AudioBuffer<float>& outputBuffer = *bufferToFill.buffer;  // Get reference to output buffer

            // Mix the microphone input with the music
            micInput.processMicInput(outputBuffer);

            // Apply gain to the mic audio to amplify its volume
            outputBuffer.applyGain(2.5f);
        }
    }

    // **Send Buffer to BeatVisualizer for Beat Detection**
    if (onAudioProcessed)
        onAudioProcessed(*bufferToFill.buffer);  // Pass the buffer for visual beat analysis

    // **Ensure Looping Works Properly**
    if (loopEnabled)
    {
        double currentPos = transportSource.getCurrentPosition();  // Get current playback position

        // If playback reaches loop end, jump back to loop start
        if (currentPos >= loopEnd)
        {
            transportSource.setPosition(loopStart);  // Reset position to loop start
            transportSource.start();  // Ensure playback resumes smoothly
            DBG("🔁 Looping back to: " + juce::String(loopStart) + "s"); // Log loop event
        }
    }
}


// **Releases Audio Resources When No Longer Needed**
void DJAudioPlayer::releaseResources()
{
    transportSource.releaseResources(); // Free transport source resources
    resampleSource.releaseResources();  // Free resampling source resources
}

// **Loads an Audio File from a Given URL**
void DJAudioPlayer::loadURL(juce::URL audioURL)
{
    DBG("🎵 Attempting to load: " + audioURL.toString(false));
    // Attempt to create an audio reader for the given file
    auto* reader = formatManager.createReaderFor(audioURL.getLocalFile());

    // Handle case where reader creation fails
    if (reader == nullptr)
    {
        DBG("DJAudioPlayer::loadURL - ERROR: Failed to create reader.");
        return;
    }

    //  Wrap the reader in a smart pointer for automatic memory management
    std::unique_ptr<juce::AudioFormatReaderSource> newSource =
        std::make_unique<juce::AudioFormatReaderSource>(reader, true);

    // Set the transport source to use the new audio reader
    transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);

    // Move the new reader source into `readerSource` (ensuring old data is cleaned up)
    readerSource = std::move(newSource);

    // Log successful file load
    DBG("✅ DJAudioPlayer::loadURL - Successfully loaded: " + audioURL.toString(false));

    // **Reset Playback State for New Track**
    stop();         // Stop current playback before loading a new track
    loopEnabled = false;  // Disable looping when a new track is loaded
    setPosition(0); //  Ensure playback starts from the beginning
    setSpeed(1.0);  // Reset playback speed to normal (1.0x) for consistency
}


// **Set Audio Gain (Volume)**
void DJAudioPlayer::setGain(double gain)
{
    gainLevel = gain;  // Store the gain value
    transportSource.setGain(gainLevel);  // Apply gain to the transport source
}

// **Set Playback Speed (Resampling Ratio)**
void DJAudioPlayer::setSpeed(double ratio)
{ 
    DBG("🎛 setSpeed() called with value: " + juce::String(ratio));
    playbackSpeed = ratio;  // Store the playback speed
    resampleSource.setResamplingRatio(playbackSpeed);  // Apply speed change

    DBG("🔍 Playback speed set to: " + juce::String(resampleSource.getResamplingRatio())); // ✅ Check applied speed

}


//  **Set Playback Position in Seconds**
void DJAudioPlayer::setPosition(double posInSecs)
{
    transportSource.setPosition(posInSecs);  // Jump to the specified time
}

// **Set Playback Position Relative to Track Length**
void DJAudioPlayer::setPositionRelative(double pos)
{
    transportSource.setPosition(pos * transportSource.getLengthInSeconds());  // ✅ Convert relative position to seconds
}

// **Pause Playback and Save Position**
void DJAudioPlayer::pause()
{
    pausedPosition = transportSource.getCurrentPosition();  // Save current playback position
    transportSource.stop();  // Stop playback without resetting position
}


// **Start Playback (Resumes from Pause if Applicable)**
void DJAudioPlayer::start()
{
    // If playback was paused, resume from the last saved position
    if (pausedPosition > 0.0)
    {
        transportSource.setPosition(pausedPosition); // Restore paused position
        pausedPosition = 0.0;  // Reset paused position after resuming
    }
    setSpeed(1.0);
    transportSource.start();  // Start playback
}

// **Stop Playback and Reset Position**
void DJAudioPlayer::stop()
{
    transportSource.stop();  // Stop playback completely
    transportSource.setPosition(0.0);  // Reset position to the start of the track
}




// **Get Playback Position Relative to Track Length**
double DJAudioPlayer::getPositionRelative()
{
    double length = transportSource.getLengthInSeconds(); // Get total track length
    return (length > 0) ? transportSource.getCurrentPosition() / length : 0.0; // ✅ Return position as a fraction (0.0 - 1.0)
}

// **Set Loop Start & End Points**
void DJAudioPlayer::setLoopPoints(double start, double end)
{
    double length = transportSource.getLengthInSeconds(); //  Get total track length

    // Convert relative positions (0.0 - 1.0) to actual time in seconds
    loopStart = start * length;
    loopEnd = end * length;

    // Debug message to confirm loop points
    DBG("🔁 Loop set from " + juce::String(loopStart) + "s to " + juce::String(loopEnd) + "s");
}

// **Enable or Disable Looping**
void DJAudioPlayer::enableLoop(bool isEnabled)
{
    loopEnabled = isEnabled; //  Toggle loop mode
}


// **Save the Current Position as a Cue Point**
void DJAudioPlayer::setCuePoint()
{
    cuePoint = transportSource.getCurrentPosition(); // Store the current playback position
    DBG("🎯 Cue Point Set at " + juce::String(cuePoint) + "s"); // Log cue point
}

// **Jump Back to the Saved Cue Point**
void DJAudioPlayer::jumpToCuePoint()
{
    transportSource.setPosition(cuePoint); // Move playback position to the saved cue point
    DBG("🚀 Jumped to Cue Point: " + juce::String(cuePoint) + "s"); // Log jump action
}


// **Check if Looping is Enabled**
bool DJAudioPlayer::isLoopEnabled() const
{
    return loopEnabled; // Return loop status (true = enabled, false = disabled)
}

// **Adjust Scratch Speed (Turntable Effect)**
void DJAudioPlayer::setScratchSpeed(double speedFactor)
{
    resampleSource.setResamplingRatio(speedFactor); // Modify playback speed dynamically
}

// **Enable or Disable Microphone Input**
void DJAudioPlayer::enableMic(bool enable)
{
    micEnabled = enable; // Store microphone state

    if (micEnabled)
        micInput.start();  //  Start capturing mic input
    else
        micInput.stop();   // Stop capturing mic input
}
