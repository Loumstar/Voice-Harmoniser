#include <SoftwareSerial.h>
#include <math.h>
#include <midi.h>
#include <audio_out.h>

#define AUDIO_IN A0
#define AUDIO_OUT 3

#define MIDI_IN 5
#define MIDI_OUT 6

#define PITCH_DETECTOR_IN 10
#define PITCH_DETECTOR_OUT 11

SoftwareSerial midiDevice(MIDI_IN, MIDI_OUT);
SoftwareSerial pitchDetector(PITCH_DETECTOR_IN, PITCH_DETECTOR_OUT);

note notes[MAX_VOICES];

int midi_msg[3]; // max length of midi message is 3.
char serial_msg[100];
double sample[SAMPLE_FRAMES];

double voice_f;

size_t frame = 0;

void setup(){
    Serial.begin(9600); // USB baud rate
    while(!Serial); // While USB connection has not been established
    Serial.write("USB connection established");
    
    midiDevice.begin(31250); // MIDI baud rate
    while(!midiDevice);
    Serial.println("MIDI Device connection established");

    pitchDetector.begin(9600); // Arduino baud rate
    while(!pitchDetector);
    Serial.println("Pitch Detector Arduino connection established");

    sprintf(
        serial_msg, 
        "Initial Setup:\nSampling Rate: %d Hz.\nSample length %d ms.\n%d frames per sample.", 
        SAMPLE_RATE, 
        (int) LATENCY * 1000, 
        SAMPLE_FRAMES
    );
    Serial.println(serial_msg);

    // Do not start processing signal until voice frequency determined
    while(!pitchDetector.available()){ 
        pitchDetector.listen();
    }
    voice_f = pitchDetector.parseFloat();
    
    sprintf(serial_msg, "New voice frequency: %d", voice_f);
    Serial.println(serial_msg);

    pinMode(AUDIO_IN, INPUT);
    pinMode(AUDIO_OUT, OUTPUT);
}

void loop(){
    midiDevice.listen();
    if(midiDevice.available()){
        read_midi(midi_msg, midiDevice);
        handle_midi(midi_msg, notes);
        
        sprintf(serial_msg, "Midi update: %x %x %x.", midi_msg[0], midi_msg[1], midi_msg[2]);
        Serial.println(serial_msg);
    }

    pitchDetector.listen();
    if(pitchDetector.available()){
        voice_f = pitchDetector.parseFloat();

        sprintf(serial_msg, "New voice frequency: %f", voice_f);
        Serial.println(serial_msg);
    }

    frame = 0;
    while(frame < SAMPLE_FRAMES){ // records the sample
        sample[frame] = analogRead(AUDIO_IN) / 1024;
        delay(pow(SAMPLE_RATE, -1) * 1000);
        frame++;
    }

    frame = 0;
    while(frame < SAMPLE_FRAMES){ // plays back sample
        digitalWrite(AUDIO_OUT, playback_amplitude(sample, frame, voice_f, notes) * 255);
        delay(pow(SAMPLE_RATE, -1) * 1000);
        frame++;
    }
}