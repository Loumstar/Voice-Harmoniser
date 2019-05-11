#include <stdbool.h>
#include "fourier_transform.h"
#include "signal_methods.h"
#include "peaks_correlation.h"

#define THRESHOLD 3.0

#define SAMPLE_ARR_SIZE 75
#define PEAKS_ARR_SIZE 20

//The smallest acceptable value for a floating point that is considered
//to be greater than zero.
#define FLOAT_EPSILON 0.01

bool _is_non_zero(double a){
    return a > FLOAT_EPSILON;
}

bool _is_maxima(double y0, double y1, double y2){
    return (y1 - y0 > FLOAT_EPSILON) && (y1 - y2 > FLOAT_EPSILON);
}

bool _is_above_threshold(double a, double noise){
    return a / noise > THRESHOLD || noise < FLOAT_EPSILON;
}

double decibels(double v){
    //base voltage is one as the amplitude is scaled to between 0 and 1.
    return 20 * log10f(v);
}

double get_noise_level(int f, double complex clip[]){
    /*
    Method to determine the amplitude of frequencies surrounding f, known as the 'noise'.
    This aids in determining whether f is a peak as if its amplitude is significantly 
    larger, the ratio of its ampltiude to noise will be larger than the threshold, which
    will add it to the peaks array.

    This is done by summing the amplitudes that fall within the sample surrounding f,
    and is dviding by the sample array size.

    */
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
        sum += cabs(clip[i]);
    }
    return (double) sum / SAMPLE_ARR_SIZE;
}

void _convert_to_frequency_domain(double complex clip[]){
    double offset = mean(clip, CLIP_FRAMES);
    remove_offset(clip, offset);
    fft(clip, CLIP_FRAMES);
}

frequency_bin* get_peaks(double complex clip[]){
    frequency_bin* peaks = malloc(FREQUENCY_BIN_SIZE * PEAKS_ARR_SIZE);
    if(peaks == NULL){
        printf(
            "Malloc error at get_peaks() method: failed to allocate %zu bytes.\n",
            FREQUENCY_BIN_SIZE * PEAKS_ARR_SIZE
        );
        return NULL;
    }
    
    double noise, amplitude;
    size_t i = 0;
    for(size_t f = 0; f < floor(CLIP_FRAMES / 2); f++){
        noise = get_noise_level(f, clip);
        amplitude = cabs(clip[f]);
        if(
            i < PEAKS_ARR_SIZE
            && _is_non_zero(amplitude)
            && _is_maxima(cabs(clip[f-1]), amplitude, cabs(clip[f+1]))
            && _is_above_threshold(amplitude, noise)
        ){
            peaks[i][0] = (double) f * FRAME_RATE / CLIP_FRAMES;
            peaks[i][1] = (double) decibels(amplitude * 2 / CLIP_FRAMES);
            i++;
        }
    }
    while(i < PEAKS_ARR_SIZE){ //initialises unused positions in peaks[] to a a standard form.
        peaks[i][0] = NAN;
        peaks[i][1] = -1 * INFINITY;
        peaks[i][2] = NAN;
        i++;
    }
    return peaks;
}

frequency_bin* get_pitch(double complex clip[]){
    _convert_to_frequency_domain(clip);
    frequency_bin* peaks = get_peaks(clip);
    
    if(peaks != NULL){
        note_probabilities(peaks, PEAKS_ARR_SIZE, FLOAT_EPSILON);
    }

    return peaks;
}