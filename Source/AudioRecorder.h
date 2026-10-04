// Ensures this header file is included only once in a single compilation
#pragma once 

// Includes the JUCE library, which provides audio processing capabilities
#include <JuceHeader.h>

// Defines the AudioRecorder class, which implements the juce::AudioSource interface
class AudioRecorder : public juce::AudioSource
{
public:
    // Starts recording audio and saves it to the specified file
    void startRecording(const juce::File& file);

    // Stops the recording process
    void stopRecording();

    // Checks if recording is currently in progress (returns true if a writer exists)
    bool isRecording() const { return writer != nullptr; }

    // Called before playback starts; used to allocate resources and initialize settings
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

    // Fills the provided audio buffer with the next chunk of audio data
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

    // Releases any allocated resources when playback stops
    void releaseResources() override;

private:
    // Pointer to the audio writer, which handles writing audio data to a file
    std::unique_ptr<juce::AudioFormatWriter> writer;

    // Pointer to the file output stream, which manages writing data to disk
    std::unique_ptr<juce::FileOutputStream> outputStream;

    // Temporary buffer for storing audio samples before writing them to the file
    juce::AudioBuffer<float> buffer;
};
