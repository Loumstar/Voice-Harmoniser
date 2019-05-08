#include <complex.h>

#define FRAME_RATE 4096
#define BIT_DEPTH 1
#define CLIP_LENGTH 1

#define CLIP_FRAMES (size_t) FRAME_RATE * CLIP_LENGTH //must be a power of 2

double mean(double complex arr[], size_t length){
    double sum = 0;
    for(size_t i = 0; i < length; i++){
        sum += arr[i];
    }
    return (double) sum / length;
}

void remove_offset(double complex clip[], double offset){
    for(size_t i = 0; i < CLIP_FRAMES; i++){
        clip[i] = (clip[i] - offset) / BIT_DEPTH;
    }
}