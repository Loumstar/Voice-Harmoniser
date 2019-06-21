#include "/Users/louismanestar/Documents/vocal_harmoniser/c/arduino/pitch_detection_lib_modified/pitch_detection.h"
#include <SoftwareSerial.h>

#define AUDIO_IN A0

#define MALLOC_ERR_LED 1

#define FREQ_IN 10
#define FREQ_OUT 11

SoftwareSerial samplerArduino(FREQ_IN, FREQ_OUT);

complex audio_signal[CLIP_FRAMES];
double f;

size_t i = 0;

void setup(){
    pinMode(AUDIO_IN, INPUT);
    pinMode(MALLOC_ERR_LED, OUTPUT);

    samplerArduino.begin(9600);
    while(!samplerArduino);
}

void loop(){
    if(i == CLIP_FRAMES){
        f = get_pitch(audio_signal);
        
        if(f == 0.0) digitalWrite(MALLOC_ERR_LED, HIGH);
        
        samplerArduino.write(f);
        i = 0;
    } else {
        audio_signal[i][0] = analogRead(AUDIO_IN);
        delay(pow(FRAME_RATE, -1) * 1000); //delays by the length of a frame in miliseconds
        i++;
    }
}