#include <SoftwareSerial.h>
#include <pitch_detection.no_malloc.h>

/*
Input pins of the L register form an 8-bit binary number using L7-L0 (42-35)
Therefore the audio input can be measured accurately without noise.

The pins have the corresponding values:
L7   L6   L5   L4   L3   L2   L1   L0
128  64   32   16   8    4    2    1
*/

#define AUDIO_IN A0
#define NOT_AUDIO_IN A1

#define FREQ_IN 10
#define FREQ_OUT 11

SoftwareSerial samplerArduino(FREQ_IN, FREQ_OUT);

complex audio[SAMPLE_FRAMES];
complex audio_copy[SAMPLE_FRAMES];

frequency_bin notes[NOTES_ARR_SIZE];
double harmonics[HARMONICS_ARR_SIZE];

double voice_f;

void setup(){
    samplerArduino.begin(9600);
    while(!samplerArduino);
}

void loop(){
    for(size_t i = 0; i < SAMPLE_FRAMES; i++){
        audio[i][0] = analogRead(AUDIO_IN) / 4; // Work out how to use NOT_AUDIO_IN             
        delay(pow(FRAME_RATE, -1) * 1000); // Delays by the length of a frame in miliseconds
    }

    voice_f = get_pitch(audio, audio_copy, notes, harmonics);    
    samplerArduino.write(voice_f);
}