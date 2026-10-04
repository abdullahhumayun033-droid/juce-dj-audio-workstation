#include "AudioRecorder.h"

// Starts recording audio and saves it to the specified file
void AudioRecorder::startRecording(const juce::File& file) {
    stopRecording();  // Stop any ongoing recording before starting a new one

    // Create an output stream for writing audio data to the file
    outputStream = file.createOutputStream();

    // Ensure the output stream was successfully created
    if (outputStream) {
        juce::WavAudioFormat wavFormat;  // Use WAV format for recording

        // Create an audio writer to handle writing the recorded audio data
        writer.reset(wavFormat.createWriterFor(outputStream.get(),  // Use get() to pass raw pointer
            44100,  // Set the sample rate to 44.1 kHz (CD quality)
            2,      // Use 2 channels (stereo recording)
            16,     // Set bit depth to 16-bit
            {},     // No additional metadata
            0));    // Default quality settings

        // Check if the writer was created successfully
        if (!writer) {
            DBG("AudioRecorder::startRecording - ERROR: Failed to create writer."); // Log an error message
            outputStream.reset();  // Cleanup if writer creation fails
        }
        else {
            DBG("AudioRecorder::startRecording - Recording started."); // Log success message
        }
    }
}


// Stops recording by resetting the writer and releasing resources
void AudioRecorder::stopRecording() {
    writer.reset();  // Reset the writer to stop writing and free resources
}

// Prepares the recorder for playback by setting up the audio buffer
void AudioRecorder::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    // Allocate space for a 2-channel (stereo) audio buffer based on the expected block size
    buffer.setSize(2, samplesPerBlockExpected);
}


// Processes the next block of audio data and writes it to the file if recording is active
void AudioRecorder::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    // Ensure recording is active by checking if the writer exists
    if (writer != nullptr) {
        // Check if the buffer contains any samples
        if (bufferToFill.buffer->getNumSamples() == 0) {
            DBG("❌ No audio samples received by recorder!");  // Log a warning if no samples are received
            return;
        }

        // Check if the buffer contains actual audio (not just silence)
        float sum = 0.0f;
        for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel) {
            const float* channelData = bufferToFill.buffer->getReadPointer(channel);
            for (int i = 0; i < bufferToFill.numSamples; ++i) {
                sum += fabs(channelData[i]);  // Sum absolute sample values to check for audio presence
            }
        }

        // Log messages to indicate whether audio is being recorded or just silence
        if (sum == 0.0f) {
            DBG("⚠️ Recorder is capturing silence!");  // Warn if only silence is being recorded
        }
        else {
            DBG("🎙️ Recorder is capturing audio!");  // Indicate audio is being recorded
        }

        // Copy audio data from input buffer to the internal buffer
        buffer.copyFrom(0, 0, bufferToFill.buffer->getReadPointer(0), bufferToFill.numSamples); // Copy left channel
        buffer.copyFrom(1, 0, bufferToFill.buffer->getReadPointer(1), bufferToFill.numSamples); // Copy right channel

        // Write the buffered audio data to the output file
        writer->writeFromAudioSampleBuffer(buffer, 0, bufferToFill.numSamples);
    }
}

// Releases resources by clearing the internal audio buffer
void AudioRecorder::releaseResources() {
    buffer.setSize(0, 0);  // Reset the buffer size to free memory
}

