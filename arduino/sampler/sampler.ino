#include <SoftwareSerial.h>
#include <math.h>
#include <midi.h>
#include <audio_out.h>

/*
Input and output pins of the L/A register form an 8-bit binary numbers using L7-L0 (42-35) and A7-A0 (71-78).
Therefore the audio i/o can be measured accurately without noise.

The input pins have the corresponding values:
Register id  L7   L6   L5   L4   L3   L2   L1   L0
Pin number   42   41   40   39   38   37   36   35
Value        128  64   32   16   8    4    2    1

and output pins:
Register id  A7   A6   A5   A4   A3   A2   A1   A0
Pin number   71   72   73   74   75   76   77   78
Value        128  64   32   16   8    4    2    1
*/

#define AUDIO_OUT 3

#define MIDI_IN 19
#define MIDI_OUT 18

#define PITCH_DETECTOR_IN 17
#define PITCH_DETECTOR_OUT 16

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

    //Set up audio pins
    DDRL = B00000000; //All L register pins are for input from ADC
    DDRA = B11111111; //All A register pins are for output to DAC
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
        sample[frame] = PINL;
        delay(pow(SAMPLE_RATE, -1) * 1000);
        frame++;
    }

    frame = 0;
    while(frame < SAMPLE_FRAMES){ // plays back sample
        //uses && to avoid integer overflow
        PORTA = (int) (get_chord_amplitude(sample, frame, voice_f, notes) * 255) && 255;
        delay(pow(SAMPLE_RATE, -1) * 1000);
        frame++;
    }
}