// Ensures this header file is only included once during compilation
#pragma once  

// Includes the JUCE framework, which provides GUI and audio processing functionality
#include <JuceHeader.h>

// Defines the BeatVisualizer class, which is a GUI component that also runs a timer
class BeatVisualizer : public juce::Component, private juce::Timer
{
public:
    // Constructor: Initializes the visualizer
    BeatVisualizer();

    // Destructor: Cleans up resources when the object is destroyed
    ~BeatVisualizer() override;

    // Called whenever the component needs to repaint itself
    void paint(juce::Graphics&) override;

    // Called when the component is resized to adjust its layout
    void resized() override;

    // Processes an incoming audio buffer to detect beats
    void processAudioBuffer(juce::AudioBuffer<float>& buffer);

private:
    // Structure to store information about detected beats
    struct BeatCircle {
        float timestamp;  // The time when the beat occurred
        float intensity;  // The strength of the detected beat
        int x, y;         // Random position for visualizing the beat
    };

    // Stores a list of detected beats for visualization
    std::vector<BeatCircle> beats;

    // Stores the timestamp of the last detected beat
    float lastBeatTime = 0.0f;

    // Sensitivity threshold for detecting beats (lower value = more sensitive)
    float beatThreshold = 0.03f;

    // Detects beats within the given audio buffer
    void detectBeats(juce::AudioBuffer<float>& buffer);

    // Timer callback function that is called periodically to update the visualization
    void timerCallback() override;

    // Prevents copying and detects memory leaks in JUCE applications
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BeatVisualizer)
};
