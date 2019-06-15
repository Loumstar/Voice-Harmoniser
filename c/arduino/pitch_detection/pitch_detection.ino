#include "/Users/louismanestar/Documents/vocal_harmoniser/c/pitch_detection/pitch_detection.h"

#define MICROPHONE_IN A0
#define FREQUENCY_OUT 3

#define MALLOC_ERROR_LED 1

#define MAX_F 1000

double complex* audio_signal = malloc(sizeof(double complex) * CLIP_FRAMES);
size_t i = 0;

void setup(){
    pinMode(MICROPHONE_IN, INPUT);
    pinMode(FREQUENCY_OUT, OUTPUT);
    pinMode(MALLOC_ERROR_LED, OUTPUT);

    if(audio_signal == NULL){
        digitalWrite(MALLOC_ERROR_LED, HIGH);
    }
}

void loop(){
    if(audio_signal != NULL){
        if(i == CLIP_FRAMES - 1){
            f = get_pitch(audio_signal);
            if(f == 0.0) digitalWrite(MALLOC_ERROR_LED, HIGH);

            analogWrite(FREQUENCY_OUT, (int) f / MAX_F * 255);
            i = 0;
        } else {
            audio_signal[i] = analogRead(MICROPHONE_IN);
            i++;
            
            delay(pow(FRAME_RATE, -1) * 1000); //delays by the length of a frame in miliseconds
        }
    }
}