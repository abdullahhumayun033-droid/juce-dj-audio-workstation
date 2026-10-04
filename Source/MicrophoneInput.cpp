#include "MicrophoneInput.h"

// **Microphone Input Constructor (Initializes Audio Device)**
MicrophoneInput::MicrophoneInput()
{
    // Initialize the audio device with:
    // - 1 input channel (for microphone)
    // - 2 output channels (for stereo playback)
    audioDeviceManager.initialiseWithDefaultDevices(1, 2);

    // Register this class to receive audio callbacks
    audioDeviceManager.addAudioCallback(&audioSourcePlayer);
}

// ❌ **Destructor: Cleanup Audio Resources**
MicrophoneInput::~MicrophoneInput()
{
    audioDeviceManager.removeAudioCallback(&audioSourcePlayer); // Unregister callback to prevent issues
}

// **Start Capturing Microphone Input**
void MicrophoneInput::start()
{
    audioDeviceManager.addAudioCallback(&audioSourcePlayer); // Reattach callback to start audio processing
}

// **Stop Capturing Microphone Input**
void MicrophoneInput::stop()
{
    audioDeviceManager.removeAudioCallback(&audioSourcePlayer); // Detach callback to stop processing
}


// **Process Microphone Input and Mix with Main Audio Buffer**
void MicrophoneInput::processMicInput(juce::AudioBuffer<float>& outputBuffer)
{
    // If there's no mic input, exit early to avoid unnecessary processing
    if (micBuffer.getNumSamples() == 0) return;

    int numSamples = outputBuffer.getNumSamples();   // Get the number of audio samples in the output buffer
    int numChannels = outputBuffer.getNumChannels(); // Get the number of audio channels (stereo or mono)

    // **Loop Through Each Channel and Mix Mic Input**
    for (int ch = 0; ch < numChannels; ++ch)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            // Ensure we don't exceed mic buffer size
            if (i < micBuffer.getNumSamples())
            {
                float micSample = micBuffer.getSample(ch, i) * 0.8f; // Reduce mic volume to 80% for better mixing
                outputBuffer.addSample(ch, i, micSample); // Mix microphone input with main audio
            }
        }
    }
}





// **Process Microphone Input and Mix with Output Audio**
void MicrophoneInput::audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
    float** outputChannelData, int numOutputChannels,
    int numSamples)
{
    // Ensure there is at least one input (mic) and one output (speakers)
    if (numInputChannels > 0 && numOutputChannels > 0)
    {
        // **Loop Through Each Output Channel**
        for (int channel = 0; channel < numOutputChannels; ++channel)
        {
            // **Process Each Audio Sample**
            for (int sample = 0; sample < numSamples; ++sample)
            {
                // Get microphone input (use first input channel, defaulting to 0.0f if unavailable)
                float micInput = (inputChannelData != nullptr && inputChannelData[0] != nullptr)
                    ? inputChannelData[0][sample]
                    : 0.0f;

                // Reduce mic volume to 50% to balance levels
                float mixedOutput = micInput * 0.5f;

                // Mix microphone input into the output buffer if it's valid
                if (outputChannelData != nullptr && outputChannelData[channel] != nullptr)
                {
                    outputChannelData[channel][sample] += mixedOutput;  // Add mic audio to the output stream
                }
            }
        }
    }
}

// **Prepare Audio Device Before Starting (Can Be Used for Initialization)**
void MicrophoneInput::audioDeviceAboutToStart(juce::AudioIODevice* device) {}

// ❌ **Called When Audio Device Stops (Can Be Used for Cleanup)**
void MicrophoneInput::audioDeviceStopped() {}
