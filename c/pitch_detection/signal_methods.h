#include <complex.h>

#define FRAME_RATE pow(2, 14)
#define BIT_DEPTH 256

#define CLIP_FRAMES 4096 //must be a power of 2

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