#include <SoftwareSerial.h>

#include "int_complex.h"
#include "pitch_detection.h"
#include "frequency_bin_typedef.h"

#define AUDIO_IN A0

#define SAMPLER_IN 10
#define SAMPLER_OUT 11

SoftwareSerial samplerArduino(SAMPLER_IN, SAMPLER_OUT);

int_complex audio[PD_SAMPLE_ARR_SIZE];
int_complex audio_copy[PD_SAMPLE_ARR_SIZE];

frequency_bin notes[PD_NOTES_ARR_SIZE];
double harmonics[PD_HARMONICS_ARR_SIZE];

double voice_f;

void setup(){
    samplerArduino.begin(9600);
    while(!samplerArduino);
}

void loop(){
    for(size_t i = 0; i < PD_SAMPLE_ARR_SIZE; i++){
        audio[i][0] = analogRead(AUDIO_IN) / 4;          
        delay(pow(PD_SAMPLE_RATE, -1) * 1000); // Delays by the length of a frame in miliseconds
    }
    voice_f = get_pitch(audio, audio_copy, notes, harmonics);    
    samplerArduino.write(voice_f);
}