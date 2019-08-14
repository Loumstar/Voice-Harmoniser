#include <SoftwareSerial.h>
#include <math.h>
#include <stdio.h>
#include <midi.h>
#include <audio_out.h>

#define AUDIO_IN A0
#define NOT_AUDIO_IN A1

#define AUDIO_OUT 15
#define NOT_AUDIO_OUT 16

#define MIDI_IN 19
#define MIDI_OUT 18

#define PITCH_DETECTOR_IN 17
#define PITCH_DETECTOR_OUT 16

SoftwareSerial midiDevice(MIDI_IN, MIDI_OUT);
SoftwareSerial pitchDetector(PITCH_DETECTOR_IN, PITCH_DETECTOR_OUT);

note notes[MAX_VOICES];

uint8_t sample[SAMPLE_FRAMES];
uint8_t amplitude;

int midi_msg[3];
char arduino_status_msg[100];

double voice_f;

void setup(){
    Serial.begin(9600); // USB baud rate
    while(!Serial); // While USB connection has not been established
    Serial.print("USB connection established\n");
    
    midiDevice.begin(31250); // MIDI baud rate
    while(!midiDevice);
    Serial.print("MIDI Device connection established\n");

    pitchDetector.begin(9600); // Arduino baud rate
    while(!pitchDetector);
    Serial.print("Pitch Detector Arduino connection established\n");

    sprintf(
        arduino_status_msg, 
        "Initial Setup:\nSampling Rate: %d Hz.\nSample length %d ms.\n%d frames per sample.\n", 
        SAMPLE_RATE, 
        (int) LATENCY * 1000, 
        SAMPLE_FRAMES
    );

    Serial.print(arduino_status_msg);

    // Do not start processing signal until voice frequency determined
    while(!pitchDetector.available()){ 
        pitchDetector.listen();
    }

    voice_f = pitchDetector.parseFloat();
    
    sprintf(arduino_status_msg, "New voice frequency: %d\n", voice_f);
    Serial.print(arduino_status_msg);

    // Set up audio output pins
    pinMode(AUDIO_OUT, OUTPUT);
    pinMode(NOT_AUDIO_OUT, OUTPUT);
}

void loop(){
    midiDevice.listen();
    if(midiDevice.available()){
        get_midi_msg(midi_msg, midiDevice);
        handle_midi_msg(midi_msg, notes);
        report_midi_change(midi_msg, arduino_status_msg);
        Serial.print(arduino_status_msg);
    }

    pitchDetector.listen();

    if(pitchDetector.available()){
        voice_f = pitchDetector.parseFloat();

        sprintf(arduino_status_msg, "New voice frequency: %f\n", voice_f);
        Serial.print(arduino_status_msg);
    }

    for(size_t frame = 0; frame < SAMPLE_FRAMES; frame++){ // Record the sample frame by frame
        sample[frame] = analogRead(AUDIO_IN) / 4; // Work out how to use NOT_AUDIO_OUT to reduce noise
        delay(pow(SAMPLE_RATE, -1) * 1000);
    }

    for(size_t frame = 0; frame < SAMPLE_FRAMES; frame++){ // Play back sample frame by frame
        // If voice_f is zero, then the audio input from PINL is fed directly to PORTA
        amplitude = voice_f ? combined_notes_amplitude_8bit(sample, notes, voice_f, frame, SAMPLE_FRAMES) : sample[frame];
        
        analogWrite(AUDIO_OUT, amplitude);
        analogWrite(NOT_AUDIO_OUT, ~amplitude);

        delay(pow(SAMPLE_RATE, -1) * 1000);
    }
}