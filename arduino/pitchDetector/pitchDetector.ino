#include <SoftwareSerial.h>
#include <pitch_detection.h>

/*
Input pins of the L register form an 8-bit binary number using L7-L0 (42-35)
Therefore the audio input can be measured accurately without noise.

The pins have the corresponding values:
L7   L6   L5   L4   L3   L2   L1   L0
128  64   32   16   8    4    2    1
*/

#define MALLOC_ERR_LED 1

#define FREQ_IN 10
#define FREQ_OUT 11

SoftwareSerial samplerArduino(FREQ_IN, FREQ_OUT);

complex audio_signal[CLIP_FRAMES];
double f;

size_t i = 0;

void setup(){
    DDRL = B00000000; //pins L0-L7 are input
    pinMode(MALLOC_ERR_LED, OUTPUT);

    samplerArduino.begin(9600);
    while(!samplerArduino);
}

void loop(){
    if(i == CLIP_FRAMES){
        f = get_pitch(audio_signal);
        
        if(!f) digitalWrite(MALLOC_ERR_LED, HIGH);
        
        samplerArduino.write(f);
        i = 0;
    } else {
        audio_signal[i][0] = PINL;
                          
        delay(pow(FRAME_RATE, -1) * 1000); //delays by the length of a frame in miliseconds
        i++;
    }
}