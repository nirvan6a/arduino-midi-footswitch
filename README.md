# Footswitch

A three-button Bluetooth MIDI foot controller built with an Arduino Uno R4 WiFi and a custom 3D-printed enclosure. Built for switching patches and toggling the tuner in Apple MainStage (or any DAW, like Ableton Live); the buttons send MIDI CC messages that can be mapped to other controls.

## Getting started

1. Assemble the switches and Arduino using the project wiring and enclosure design.
2. Upload the Arduino sketch using Arduino IDE.
3. On macOS, connect the controller through **Audio MIDI Setup → MIDI Studio → Bluetooth**.
4. In MainStage, map the three buttons to your desired actions.


## Example Usecases

1. With a MainStage concert, it can be used for three simple actions: tuner, previous patch, and next patch, allowing hands-free navigation through patches during a performance
2. With a looper plugin (such as Loopback in MainStage), Logic Live Loops or Ableton Live, it can be mapped to record a loop, trigger a loop, and switch loop banks, becoming a complete looper station

[Build notes, photos and demos](https://nirvan.blog/midi-footswitch)
