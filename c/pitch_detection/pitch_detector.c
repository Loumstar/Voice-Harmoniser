#include <stdbool.h>
#include "./fourier_transform.h"

#define FRAME_RATE 4096
#define BIT_DEPTH 1

#define CLIP_LENGTH 1

#define CLIP_FRAMES (size_t) round(FRAME_RATE * CLIP_LENGTH)

#define THRESHOLD 3

#define DISTRIBUTION_SPACING 50

#define HARMONICS_ARR_SIZE (size_t) 20
#define SAMPLE_ARR_SIZE (size_t) 75
#define PEAKS_ARR_SIZE (size_t) 20

#define EULER M_E

void print_complex_array(double complex complex_arr[], size_t s){
    for(size_t j = 0; j < s; j++){
        printf("%f + i%f\n", creal(complex_arr[j]), cimag(complex_arr[j]));
    }
}

void print_magnitude(double complex complex_arr[], size_t s){
    for(size_t j = 0; j < s; j++){
        printf("%lu Hz: %f\n", j, cabs(complex_arr[j]));
    }
}

int* get_spectrum(){
    int* spectrum = malloc(sizeof(int) * CLIP_FRAMES);
    for(size_t i = 0; i < floor(CLIP_FRAMES / 2); i++){
        spectrum[i] = i / CLIP_LENGTH;
    }
    return spectrum;
}

double mean(double complex arr[], size_t length){
    double sum = 0;
    for(size_t i = 0; i < length; i++){
        sum += arr[i];
    }
    printf("Offset: %f\n", sum);
    return (double) sum / length;
}

void remove_offset(double complex clip[], double offset){
    for(size_t i = 0; i < CLIP_FRAMES; i++){
        clip[i] = (clip[i] - offset) / BIT_DEPTH;
    }
}

void _decompose(double complex clip[]){
    double offset = mean(clip, CLIP_FRAMES);
    remove_offset(clip, offset);
    fft(clip, CLIP_FRAMES);
    print_complex_array(clip, 10);
    print_magnitude(clip, 100);
}

double get_noise_level(int f, double complex clip[]){
    double sum = 0;
    size_t lb, ub;
    if(f + floor(SAMPLE_ARR_SIZE / 2) > CLIP_FRAMES){
        lb = CLIP_FRAMES - SAMPLE_ARR_SIZE;
        ub = CLIP_FRAMES;
    } else if(f - floor(SAMPLE_ARR_SIZE / 2) < 0){
        lb = 0;
        ub = SAMPLE_ARR_SIZE;
    } else {
        lb = f - floor(SAMPLE_ARR_SIZE / 2);
        ub = f + floor(SAMPLE_ARR_SIZE / 2);
    }
    for(size_t i = lb; i < ub; i++){
        sum += cimag(clip[i]);
    }
    return (double) sum / SAMPLE_ARR_SIZE;
}

bool _is_maxima(double y0, double y1, double y2){
    return (y1 - y0 > 0) && (y2 - y1 < 0);
}

double* get_harmonics(double peak){
    double* harmonics = malloc(sizeof(double) * HARMONICS_ARR_SIZE);
    for(int h = 0; h < HARMONICS_ARR_SIZE; h++){
        harmonics[h] = peak * (h + 1);
    }
    return harmonics;
}

double get_correlation(double f, double complex peaks[]){
    double c = 0;
    for(size_t s = 0; s < PEAKS_ARR_SIZE; s++){
        c += pow(EULER, (2 * (f - creal(peaks[s]) / DISTRIBUTION_SPACING)));
    }
    return c;
}

double test_harmonics(double complex peaks[], double harmonics[]){
    double correlation = 0;
    for(int h = 0; h < HARMONICS_ARR_SIZE; h++){
        correlation += get_correlation(harmonics[h], peaks);
    }
    return correlation / HARMONICS_ARR_SIZE;
}

void get_peaks(double complex clip[]){
    double noise;
    for(size_t f = 0; f < CLIP_FRAMES; f++){
        noise = get_noise_level(f, clip);
        if(cimag(clip[f]) / noise > THRESHOLD && 
        _is_maxima(cimag(clip[f-1]), cimag(clip[f]), cimag(clip[f+1]))){
            clip[f] = creal(clip[f]) + (I * creal(clip[f]) / noise);
        } else {
            clip[f] = 0;
        }
    }
}

int sort_peaks(const void* a, const void* b){
    return cimag(*(double complex*)b) - cimag(*(double complex*)a);
}

void get_note_probabilities(double complex peaks[]){    
    double probability;
    for(int p = 0; p < PEAKS_ARR_SIZE; p++){
        double* harmonics = get_harmonics(creal(peaks[p]));
        probability = test_harmonics(peaks, harmonics);

        free(harmonics);

        peaks[p] = creal(peaks[p]) + (I * probability);
    }
}

void get_pitch(double complex clip[]){
    _decompose(clip);

    //printf("Decomposed:\n");
    //print_complex_array(clip, CLIP_FRAMES);

    
    //get_peaks(clip);
    //qsort(clip, CLIP_FRAMES, sizeof(double complex), sort_peaks);
    //from this point on, use PEAKS_ARR_SIZE as the array is now in descending order

    //printf("Peaks:\n");
    //print_complex_array(clip, PEAKS_ARR_SIZE);

    //get_note_probabilities(clip);
    
    //printf("Notes\n");
    //print_complex_array(clip, PEAKS_ARR_SIZE);
}