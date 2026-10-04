# OtoDecks – JUCE DJ Audio Workstation

A desktop DJ/audio application built in **C++ with the JUCE framework** as an Object Oriented Programming project in the University of London BSc Computer Science programme.

The project extends a two-deck audio player into a more complete DJ workstation, with reusable classes for playback, waveform rendering, playlist management, recording, microphone input, visual feedback and UI theming.

## Features

- Two independent audio decks
- Play, pause, stop and file loading controls
- Volume, playback-speed and position controls
- Crossfader for blending both decks
- Waveform rendering with live playhead position
- Drag-and-drop audio loading
- Playlist with load-to-deck and remove actions
- Cue-point support
- Loop-point controls
- Turntable-style waveform scratching / speed manipulation
- Beat-responsive visualisation
- Microphone input support
- Mixed-output WAV recording
- Light, dark and custom themes
- Keyboard shortcuts and responsive component layout

## Object-Oriented Design

The application is split into focused classes with separate responsibilities:

- `DJAudioPlayer` – audio loading, transport, gain, resampling, looping and cue control
- `DeckGUI` – per-deck controls and user interaction
- `WaveformDisplay` – waveform rendering, playhead display and scratch interaction
- `PlaylistComponent` – track collection and deck-loading actions
- `AudioRecorder` – WAV recording of the mixed output
- `MicrophoneInput` – microphone capture and mixing
- `BeatVisualizer` – audio-level analysis and beat-reactive graphics
- `ThemeManager` – centralised application colour/theme handling
- `MainComponent` – high-level composition, audio routing, crossfader and recording controls

This structure demonstrates encapsulation, composition, event-driven programming, inheritance from JUCE components/interfaces, callback-based interaction and separation of concerns.

## Technologies

- C++
- JUCE
- Projucer
- JUCE audio transport and resampling APIs
- JUCE GUI components
- Visual Studio 2019 / 2022 exporter configuration

## Project Structure

```text
.
├── OtoDecks.jucer
├── README.md
├── .gitignore
└── Source/
    ├── Main.cpp
    ├── MainComponent.cpp/.h
    ├── DJAudioPlayer.cpp/.h
    ├── DeckGUI.cpp/.h
    ├── WaveformDisplay.cpp/.h
    ├── PlaylistComponent.cpp/.h
    ├── AudioRecorder.cpp/.h
    ├── MicrophoneInput.cpp/.h
    ├── BeatVisualizer.cpp/.h
    ├── ThemeManager.cpp/.h
    └── resources/
        ├── play.png
        ├── pause.png
        └── stop.png
```

## Building the Project

This repository contains the original **Projucer** project configuration rather than generated IDE build files.

1. Install JUCE / Projucer.
2. Open `OtoDecks.jucer` in Projucer.
3. If Projucer reports missing JUCE module paths, point the project to the `JUCE/modules` folder on your machine.
4. Save the project to regenerate the IDE exporter files.
5. Open the generated Visual Studio project and build the application.

The stored exporter configuration was created on the original development machine, so JUCE module paths may need to be updated before building on another computer.

## Usage

Load audio into either deck using the deck load controls, drag-and-drop, or the playlist. Each deck provides playback, speed, position, cue, loop and microphone controls. The crossfader blends the two deck outputs, while the recording control can save the mixed output as a WAV file.

## Coursework Context

This repository is a cleaned portfolio version of the original Object Oriented Programming project. Generated build artefacts and machine-specific IDE metadata are intentionally excluded from version control.

## Author

**Abdullah Humayun**  
BSc Computer Science – University of London / Goldsmiths
