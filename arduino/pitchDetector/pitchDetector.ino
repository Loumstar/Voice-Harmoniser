#include <SoftwareSerial.h>
#include <pitch_detection.no_malloc.h>

#define AUDIO_IN A0
#define NOT_AUDIO_IN A1

#define SAMPLER_IN 10
#define SAMPLER_OUT 11

SoftwareSerial samplerArduino(SAMPLER_IN, SAMPLER_OUT);

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