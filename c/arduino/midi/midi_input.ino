#include <SoftwareSerial.h>
#include <math.h>

#define AUDIO_IN A0
#define AUDIO_OUT 5

#define MIDI_IN 10
#define MIDI_OUT 11

SoftwareSerial midiSerial(MIDI_IN, MIDI_OUT);
int msg[3];

void note_off(double freq);
void note_on(double freq, int vol);

double note_frequency(int note){
    return pow(2, (note - 69) / 12) * 440.0;
}

void handle_midi(int* msg){
    if(msg[0] < 0xA0){
        double f = note_frequency(msg[1]);
        if(msg[0] < 0x90){
            note_off(f);
        } else {
            note_on(f, msg[2]);
        }
    }
}

int* midi_read(int* msg, size_t msg_length){
    size_t i = 0;
    while(i < msg_length){
        msg[i] = midiSerial.read(); //need to check for status byte
        i++;
    }
    while(i < 3){
        msg[i] = 0x00;
        i++;
    }
}

void setup(){
    Serial.begin(9600); // USB baud rate
    while(!Serial); // While USB connection has not been established
    midiSerial.begin(31250); // MIDI baud rate

    pinMode
}

void loop(){
    midiSerial.listen();
    
    if(midiSerial.available()){
        midi_read(msg, midiSerial.available());
        handle_midi(msg);
    }
    
    /*
    Code to play each note based on the speed of the circular buffer.

    Requires:
    - Circular buffer read of size ??
    - A list of the speeds to combine the buffers

    Method:
    1. Loops buffer at rate of highest speed.
    2. Finds the mean amplitude (input amplitude x volume) across all buffers for each frame of the fastest buffer.
    3. Outputs the mean to AUDIO_OUT.
    4. Once input buffer is completed the loop is exited.
    */
}