#include <SoftwareSerial.h>
#include <math.h>

SoftwareSerial midiSerial(10, 11);
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
}

void loop(){
    midiSerial.listen();
    
    if(midiSerial.available()){
        midi_read(msg, midiSerial.available());
        handle_midi(msg);
    }
}