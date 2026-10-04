#include "BeatVisualizer.h"

// Constructor: Initializes the beat visualizer
BeatVisualizer::BeatVisualizer()
{
    // Start a timer that triggers every 50ms to refresh the visualization
    startTimer(50);
}

// Destructor: Cleans up resources when the object is destroyed
BeatVisualizer::~BeatVisualizer() {}

// Processes the incoming audio buffer to detect beats and trigger visualization updates
void BeatVisualizer::processAudioBuffer(juce::AudioBuffer<float>& buffer)
{
    // Analyze the audio buffer to detect beats
    detectBeats(buffer);

    // Ensure repaint runs on the GUI thread to update the visualization
    juce::MessageManager::callAsync([this]() { repaint(); });
}


// Detects beats in the given audio buffer by analyzing peak levels
void BeatVisualizer::detectBeats(juce::AudioBuffer<float>& buffer)
{
    // Return early if the buffer is empty (no audio samples)
    if (buffer.getNumSamples() == 0) return;

    int numSamples = buffer.getNumSamples();   // Get total number of samples
    int numChannels = buffer.getNumChannels(); // Get number of audio channels

    float peakLevel = 0.0f; // Store the highest detected sample value

    // Iterate through all channels and samples to find the peak level
    for (int ch = 0; ch < numChannels; ++ch)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            float sample = std::abs(buffer.getSample(ch, i)); // Get the absolute sample value
            if (sample > peakLevel)  // Check if it's the highest value so far
                peakLevel = sample;
        }
    }

    // If the peak level exceeds the beat detection threshold, register a beat
    if (peakLevel > beatThreshold)
    {
        float currentTime = juce::Time::getMillisecondCounterHiRes() / 1000.0f; // Get current time in seconds

        // Prevent too many beats from being detected in quick succession
        if (currentTime - lastBeatTime > 0.2f)
        {
            // Generate random x and y positions for visualizing the beat
            int randX = juce::Random::getSystemRandom().nextInt(getWidth());
            int randY = juce::Random::getSystemRandom().nextInt(getHeight());

            // Store detected beat with a random position and intensity scaled for visualization
            beats.push_back({ currentTime, peakLevel * 40.0f, randX, randY });

            // Update the last detected beat time
            lastBeatTime = currentTime;
        }
    }
}





// Paints the beat visualization onto the component
void BeatVisualizer::paint(juce::Graphics& g)
{
    // Fill the background with black for better contrast
    g.fillAll(juce::Colours::black);  // Keep background black

    // If no beats are detected, there's nothing to draw
    if (beats.empty()) return;

    // Get the current time in seconds
    float currentTime = juce::Time::getMillisecondCounterHiRes() / 1000.0f;

    // Iterate over all detected beats
    for (const auto& beat : beats)
    {
        // Calculate how long ago the beat was detected
        float age = currentTime - beat.timestamp;

        // Only display beats that occurred in the last 2 seconds
        if (age < 2.0f)
        {
            // Calculate transparency: older beats become more transparent
            float alpha = 1.0f - (age / 2.0f);

            // Circles grow over time for a fading effect
            float size = beat.intensity * (1.0f + age * 1.5f);

            // Set the color to yellow with decreasing opacity
            g.setColour(juce::Colours::yellow.withAlpha(alpha));

            // Draw the beat as a growing, fading circle at its random position
            g.fillEllipse(beat.x - size / 2, beat.y - size / 2, size, size);
        }
    }
}




// Called when the component is resized (not used here, but can be extended if needed)
void BeatVisualizer::resized() {}

// Periodically called by the timer to update and clean up the beat visualization
void BeatVisualizer::timerCallback()
{
    // Get the current time in seconds
    float currentTime = juce::Time::getMillisecondCounterHiRes() / 1000.0f;

    // Safely remove old beats to prevent memory issues
    std::vector<BeatCircle> newBeats; // Temporary vector to store active beats

    // Iterate through all beats and keep only the recent ones (within 2 seconds)
    for (const auto& beat : beats)
    {
        if ((currentTime - beat.timestamp) <= 2.0f)
        {
            newBeats.push_back(beat); // Retain valid beats
        }
    }

    // Replace old beats with the updated list
    beats = std::move(newBeats);

    // Ensure repaint() runs on the main (message) thread
    if (!beats.empty())
    {
        juce::MessageManager::callAsync([this]() { repaint(); }); // Asynchronously trigger repaint
    }
}


