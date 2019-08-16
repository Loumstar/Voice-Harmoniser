#include <SoftwareSerial.h>
#include <pitch_detection.no_malloc.h>

#define AUDIO_IN A0
#define NOT_AUDIO_IN A1

#define SAMPLER_IN 10
#define SAMPLER_OUT 11

SoftwareSerial samplerArduino(SAMPLER_IN, SAMPLER_OUT);

complex audio[PD_SAMPLE_ARR_SIZE];
complex audio_copy[PD_SAMPLE_ARR_SIZE];

frequency_bin notes[PD_NOTES_ARR_SIZE];
double harmonics[PD_HARMONICS_ARR_SIZE];

double voice_f;

void setup(){
    samplerArduino.begin(9600);
    while(!samplerArduino);
}

void loop(){
    for(size_t i = 0; i < PD_SAMPLE_ARR_SIZE; i++){
        audio[i][0] = analogRead(AUDIO_IN) / 4; // Work out how to use NOT_AUDIO_IN             
        delay(pow(PD_SAMPLE_RATE, -1) * 1000); // Delays by the length of a frame in miliseconds
    }

    voice_f = get_pitch(audio, audio_copy, notes, harmonics);    
    samplerArduino.write(voice_f);
}