#include <SoftwareSerial.h>
#include <pitch_detection.no_malloc.h>

/*
Input pins of the L register form an 8-bit binary number using L7-L0 (42-35)
Therefore the audio input can be measured accurately without noise.

The pins have the corresponding values:
L7   L6   L5   L4   L3   L2   L1   L0
128  64   32   16   8    4    2    1
*/

#define FREQ_IN 10
#define FREQ_OUT 11

SoftwareSerial samplerArduino(FREQ_IN, FREQ_OUT);

complex audio[CLIP_FRAMES];
complex audio_copy[CLIP_FRAMES];

frequency_bin notes[PEAKS_ARR_SIZE];
double harmonics[HARMONICS_ARR_SIZE];

double voice_f;

void setup(){
    DDRL = B00000000; // Pins L0-L7 are input

    samplerArduino.begin(9600);
    while(!samplerArduino);
}

void loop(){
    for(size_t i = 0; i < CLIP_FRAMES; i++){
        audio[i][0] = PINL;              
        delay(pow(FRAME_RATE, -1) * 1000); // Delays by the length of a frame in miliseconds
    }

    voice_f = get_pitch(audio, audio_copy, notes, harmonics);    
    samplerArduino.write(voice_f);
}