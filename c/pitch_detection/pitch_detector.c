#include <math.h>
#include <stdbool.h>
#include "./fourier_transform.h"

#define FRAME_RATE 44100
#define BIT_DEPTH pow(16, 2)

#define CLIP_LENGTH 0.1

#define CLIP_FRAMES FRAME_RATE * CLIP_LENGTH

#define THRESHOLD 3
#define PROBABILITY_THRESHOLD 0.01

#define DISTRIBUTION_SPACING 50

#define HARMONICS_ARR_SIZE 20
#define SAMPLE_ARR_SIZE 75
#define PEAKS_ARR_SIZE 20

int* get_spectrum(){
    int* s = malloc(sizeof(int) * CLIP_FRAMES);
    for(int i = 0; i < CLIP_FRAMES / 2; i++){
        s[i] = i / CLIP_LENGTH;
    }
    return s;
}

int mean(double complex arr[]){
    int sum = 0,
        length = sizeof(*arr) / sizeof(double complex);
    for(int i = 0; i < length; i++){
        sum += cimag(arr[i]);
    }
    return sum / length;
}

double complex remove_offset(double complex clip[], int offset){
    int length = sizeof(*clip) / sizeof(double complex);
    for(int i = 0; i < length; i++){
        clip[i] = creal(clip[i]) + (I * ((cimag(clip[i]) / BIT_DEPTH) - offset));
    }
    return *clip;
}

void _decompose(double complex clip[]){
    int offset = mean(clip);
    remove_offset(clip, offset);
    fft(clip);
}

double complex get_pitch(double complex clip[]){
    _decompose(clip);
    double complex peaks = get_peaks(clip);
    return get_note_probabilities(peaks);
}

double complex get_peaks(double complex clip[]){
    int length = sizeof(*clip) / sizeof(double complex);
    int peaks_count = 0;
    for(int f = 0; f < length; f++){
        double noise = get_noise_level();
        if(cimag(clip[f]) / noise > THRESHOLD &&
        _is_max_amplitude(clip[f-1], clip[f], clip[f+1])){
            peaks_count++;
        } else {
            clip[f] = (double complex) 0;
        }
    }
    double complex* peaks = malloc(sizeof(double complex) * peaks_count);
    int f = 0;
    int p = 0;
    while(p < peaks_count){
        if(clip[f] != (double complex) 0){
            peaks[p] = clip[f];
            p++;
        }
        f++;
    }
    return *peaks;
}

double get_noise_level();
bool _is_max_amplitude();
double complex get_note_probabilities();