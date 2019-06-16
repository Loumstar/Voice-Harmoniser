#include <SoftwareSerial.h>
#include <math.h>

#include "/Users/louismanestar/Documents/vocal_harmoniser/c/arduino/arduino_tools.h"

#define AUDIO_IN A0
#define AUDIO_OUT 3

#define MIDI_IN 5
#define MIDI_OUT 6

#define PITCH_DETECTOR_IN 10
#define PITCH_DETECTOR_OUT 11

#define LATENCY pow(10, -3) // 1 milisecond samples
#define SAMPLE_RATE 44100 // standard 44.1kHz sample rate
#define SAMPLE_FRAMES (size_t) SAMPLE_RATE * LATENCY

SoftwareSerial midiDevice(MIDI_IN, MIDI_OUT);
SoftwareSerial pitchDetectorArduino(PITCH_DETECTOR_IN, PITCH_DETECTOR_OUT);

note notes[MAX_VOICES];

int msg[3]; // max length of midi message is 3.
int sample[SAMPLE_FRAMES];

double base_frequency;

size_t frame = 0;

void setup(){
    Serial.begin(9600); // USB baud rate
    while(!Serial); // While USB connection has not been established
    
    midiDevice.begin(31250); // MIDI baud rate
    while(!midiDevice);

    pitchDetectorArduino.begin(9600); // Arduino baud rate
    while(!pitchDetectorArduino);

    pinMode(AUDIO_IN, INPUT);
    pinMode(AUDIO_OUT, OUTPUT);
}

void loop(){
    midiDevice.listen();
    if(midiDevice.available()){
        read_midi(msg, midiDevice);
        handle_midi(msg, notes);
    }

    pitchDetectorArduino.listen();
    if(pitchDetectorArduino.available()){
        base_frequency = pitchDetectorArduino.parseFloat();
    }
    
    while(frame < SAMPLE_FRAMES){ //records the sample
        sample[frame] = analogRead(AUDIO_IN);
        delay(pow(SAMPLE_RATE, -1) * 1000);
        frame++;
    }
    
    frame = 0;

    while(frame < SAMPLE_FRAMES){
        digitalWrite(AUDIO_OUT, playback_amplitude(sample, frame, notes));
        delay(pow(SAMPLE_RATE, -1) * 1000);
        frame++;
    }

    frame = 0;
    /*
    Code to play each note based on the speed of the circular buffer.

    Method:
    1. Loops buffer.
    2. Finds the mean amplitude (input amplitude x volume) across all buffers for each frame of the fastest buffer.
    3. Outputs the mean to AUDIO_OUT.
    4. Once input buffer is completed the loop is exited.
    */
}