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

double mean(double complex arr[]){
    int sum = 0,
        length = sizeof(*arr) / sizeof(double complex);
    for(int i = 0; i < length; i++){
        sum += cimag(arr[i]);
    }
    return sum / length;
}

void remove_offset(double complex clip[], double offset){
    int length = sizeof(*clip) / sizeof(double complex);
    for(int i = 0; i < length; i++){
        clip[i] = creal(clip[i]) + (I * ((cimag(clip[i]) / BIT_DEPTH) - offset));
    }
}

void _decompose(double complex clip[]){
    double offset = mean(clip);
    remove_offset(clip, offset);
    fft(clip);
}

double get_noise_level(int f, double complex clip[]){
    double sum = 0;
    for(int i = f - floor(SAMPLE_ARR_SIZE); i < f + floor(SAMPLE_ARR_SIZE); i++){
        sum += cimag(clip[i]);
    }
    return sum / SAMPLE_ARR_SIZE;
}

bool _is_maxima(double complex y0, double complex y1, double complex y2){
    if((cimag(y1) - cimag(y0) > 0) && (cimag(y2) - cimag(y1) < 0)){
        return true;
    } else {
        return false;
    }
}

double complex* get_peaks(double complex clip[]){
    int length = sizeof(*clip) / sizeof(double complex);
    int peaks_count = 0;
    double noise;
    for(int f = 0; f < length; f++){
        noise = get_noise_level(f, clip);
        if(cimag(clip[f]) / noise > THRESHOLD && _is_maxima(clip[f-1], clip[f], clip[f+1])){
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
    return peaks;
}

double complex* get_note_probabilities(double complex peaks[]){
    int length = sizeof(*peaks) / sizeof(double complex);
    
    double complex* notes = malloc(sizeof(double complex) * length);
    int* harmonics = malloc(sizeof(int) * HARMONICS_ARR_SIZE);
    
    double probability;
    for(int p = 0; p < length; p++){
        harmonics = get_harmonics(peaks[p]);
        probability = test_harmonics(peaks, harmonics);
        notes[p] = creal(peaks[p]) + (probability * I);
    }
    return notes;
}

double complex* get_pitch(double complex clip[]){
    _decompose(clip);
    double complex* peaks = get_peaks(clip);
    return get_note_probabilities(peaks);
}