// Ensures this header file is only included once in a single compilation
#pragma once  

// Includes the JUCE framework for audio functionality
#include <JuceHeader.h>

// Includes the microphone input handling class
#include "MicrophoneInput.h"  // Include microphone class

// Defines the DJAudioPlayer class, which handles audio playback and processing
class DJAudioPlayer : public juce::AudioSource
{
public:
    // Constructor: Initializes the player with an audio format manager
    DJAudioPlayer(juce::AudioFormatManager& formatManager);

    // Destructor: Cleans up resources when the object is destroyed
    ~DJAudioPlayer();

    // Prepares the player for playback by setting up the sample rate and buffer size
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

    // Retrieves the next block of audio data for playback
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

    // Releases resources when playback stops
    void releaseResources() override;

    // Loads an audio file from a given URL
    void loadURL(juce::URL audioURL);

    // Sets the gain (volume) level
    void setGain(double gain);

    // Adjusts the playback speed
    void setSpeed(double ratio);

    // Sets the current playback position in seconds
    void setPosition(double posInSecs);

    // Sets the playback position relative to the total length of the track
    void setPositionRelative(double pos);

    // Starts audio playback
    void start();

    // Stops audio playback
    void stop();

    // Gets the playback position relative to the total track length
    double getPositionRelative();

    // Pauses audio playback and stores the last position
    void pause(); // New pause function

    // Stores the last playback position before pausing
    double pausedPosition = 0.0;

    // Stores the current playback speed
    double playbackSpeed = 1.0;

    // Looping functions

    // Sets loop start and end points
    void setLoopPoints(double start, double end);

    // Enables or disables looping
    void enableLoop(bool isEnabled);

    // Cue functions

    // Sets a cue point at the current playback position
    void setCuePoint();

    // Jumps to the previously set cue point
    void jumpToCuePoint();

    // Returns whether looping is enabled
    bool isLoopEnabled() const;

    // Adjusts playback speed for scratching effects
    void setScratchSpeed(double speedFactor);

    // Callback function for processing audio (used for beat detection)
    std::function<void(juce::AudioBuffer<float>&)> onAudioProcessed;

    // Enables or disables microphone input
    void enableMic(bool enable);

    // Returns whether the microphone input is enabled
    bool isMicEnabled() const { return micEnabled; }

private:
    // Reference to the format manager for handling audio file formats
    juce::AudioFormatManager& formatManager;

    // Unique pointer to the audio reader source
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;

    // Transport source for controlling playback
    juce::AudioTransportSource transportSource;

    // Resampling source for adjusting playback speed
    juce::ResamplingAudioSource resampleSource{ &transportSource, false, 2 };

    // Stores the current gain (volume) level
    double gainLevel = 1.0;

    // Loop variables

    // Stores the loop start position in seconds
    double loopStart = 0.0;

    // Stores the loop end position in seconds
    double loopEnd = 0.0;

    // Indicates whether looping is enabled
    bool loopEnabled = false;

    // Cue variable

    // Stores the cue point position in seconds
    double cuePoint = 0.0;

    // Microphone input instance for handling mic audio
    MicrophoneInput micInput;

    // Stores the microphone enable/disable state
    bool micEnabled = false;

    // Prevents copying and detects memory leaks in JUCE applications
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DJAudioPlayer)
};
