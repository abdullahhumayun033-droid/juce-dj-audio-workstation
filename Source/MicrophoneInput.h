// Ensures this header file is only included once in a single compilation
#pragma once  

// Includes the JUCE framework for audio handling
#include <JuceHeader.h>

// Defines the MicrophoneInput class, which manages microphone audio input
class MicrophoneInput
{
public:
    // Constructor: Initializes the microphone input system
    MicrophoneInput();

    // Destructor: Cleans up resources when the object is destroyed
    ~MicrophoneInput();

    // Starts capturing audio from the microphone
    void start();

    // Stops capturing audio from the microphone
    void stop();

    // Remove `override` because it's not from a base class in JUCE 8
    // Processes incoming audio data from the microphone
    void audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
        float** outputChannelData, int numOutputChannels,
        int numSamples);

    // New function: Processes microphone input and copies it into an output buffer
    void processMicInput(juce::AudioBuffer<float>& outputBuffer);

    // Called when the audio device is about to start (sets up sample rate, buffer size, etc.)
    void audioDeviceAboutToStart(juce::AudioIODevice* device);

    // Called when the audio device stops
    void audioDeviceStopped();

private:
    // Manages audio input and output devices
    juce::AudioDeviceManager audioDeviceManager;

    // Handles audio processing and playback for the microphone input
    juce::AudioSourcePlayer audioSourcePlayer;

    // Mixer source for combining multiple audio sources
    juce::MixerAudioSource mixerSource;

    // Buffer for storing microphone input samples
    juce::AudioBuffer<float> micBuffer;
};
