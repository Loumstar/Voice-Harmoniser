#include <SoftwareSerial.h>
#include <math.h>

#include "/Users/louismanestar/Documents/vocal_harmoniser/c/arduino/arduino_tools.h"

#define AUDIO_IN A0
#define AUDIO_OUT 3

#define MIDI_IN 5
#define MIDI_OUT 6

#define PITCH_DETECTOR_IN 10
#define PITCH_DETECTOR_OUT 11

SoftwareSerial midiDevice(MIDI_IN, MIDI_OUT);
SoftwareSerial pitchDetectorArduino(PITCH_DETECTOR_IN, PITCH_DETECTOR_OUT);

note notes[MAX_VOICES];

int msg[3]; // max length of midi message is 3.
int sample[SAMPLE_FRAMES];

double voice_f;

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
        voice_f = pitchDetectorArduino.parseFloat();
    }
    
    while(frame < SAMPLE_FRAMES){ //records the sample
        sample[frame] = analogRead(AUDIO_IN) / ;
        delay(pow(SAMPLE_RATE, -1) * 1000);
        frame++;
    }
    
    frame = 0;

    while(frame < SAMPLE_FRAMES){
        digitalWrite(AUDIO_OUT, playback_amplitude(sample, frame, voice_f, notes) * 255);
        delay(pow(SAMPLE_RATE, -1) * 1000);
        frame++;
    }
    frame = 0;
}