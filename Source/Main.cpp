#include <JuceHeader.h>
#include "MainComponent.h"

// **OtoDecks DJ Application Class**
class OtoDecksApplication : public juce::JUCEApplication
{
public:
    // Constructor: Initializes the application
    OtoDecksApplication() {}

    // Returns the application name
    const juce::String getApplicationName() override { return "OtoDecks"; }

    // Returns the application version
    const juce::String getApplicationVersion() override { return "1.0.0"; }

    // Allows multiple instances of the application to run
    bool moreThanOneInstanceAllowed() override { return true; }

    // **Application Initialization**
    void initialise(const juce::String& commandLine) override
    {
        mainWindow.reset(new MainWindow(getApplicationName())); // Create main window
    }

    // **Shutdown: Cleanup Resources**
    void shutdown() override
    {
        mainWindow = nullptr; // Properly delete the main window instance
    }

    // **Handles System Quit Requests**
    void systemRequestedQuit() override
    {
        quit(); // Gracefully exit the application
    }

    // **Handles a Second Instance of the Application**
    void anotherInstanceStarted(const juce::String& commandLine) override
    {
        juce::Logger::writeToLog("Another instance started: " + commandLine); // ✅ Log the event
    }

    // **Main Application Window**
    class MainWindow : public juce::DocumentWindow
    {
    public:
        //  Constructor: Sets up the main application window
        MainWindow(juce::String name)
            : DocumentWindow(name,
                juce::Desktop::getInstance().getDefaultLookAndFeel()
                .findColour(juce::ResizableWindow::backgroundColourId),
                juce::DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true); // Use the system's default title bar
            setContentOwned(new MainComponent(), true); // Attach the MainComponent UI
            setResizable(true, true); // Allow resizing in both directions
            setFullScreen(true);//full screen
            setVisible(true); //  Make the window visible
        }


// **Handles Close Button Press (Quit Application)**
    void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit(); // Request application to quit
        }
    };

private:
    std::unique_ptr<MainWindow> mainWindow; // Smart pointer to manage the main window instance
};

// **Start the OtoDecks DJ Application**
START_JUCE_APPLICATION(OtoDecksApplication)
