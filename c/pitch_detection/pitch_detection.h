#include <stdbool.h>
#include "fourier_transform.h"
#include "signal_methods.h"
#include "peaks_correlation.h"

#define THRESHOLD 3.0

#define SAMPLE_ARR_SIZE (size_t) 75
#define PEAKS_ARR_SIZE (size_t) 20

#define FLOAT_COMPARISON_ERROR (double) 0.01

bool _bin_is_non_zero(double a){
    return a > FLOAT_COMPARISON_ERROR;
}

bool _is_maxima(double y0, double y1, double y2){
    return (y1 - y0 > FLOAT_COMPARISON_ERROR) && (y1 - y2 > FLOAT_COMPARISON_ERROR);
}

bool _is_above_threshold(double a, double noise){
    return a / noise > THRESHOLD || noise < FLOAT_COMPARISON_ERROR;
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
            "Malloc error at get_peaks() method: failed while requesting %zu bytes.\n",
            FREQUENCY_BIN_SIZE * PEAKS_ARR_SIZE
        );
        return NULL;
    }
    
    double noise, ampltde;
    size_t i = 0;
    
    for(size_t f = 0; f < floor(CLIP_FRAMES / 2); f++){
        noise = get_noise_level(f, clip);
        ampltde = cabs(clip[f]);
        if(
            i < PEAKS_ARR_SIZE
            && _bin_is_non_zero(ampltde)
            && _is_maxima(cabs(clip[f-1]), ampltde, cabs(clip[f+1]))
            && _is_above_threshold(ampltde, noise)
        ){
            peaks[i][0] = (double) f;
            peaks[i][1] = (double) ampltde * 2 / CLIP_FRAMES;
            i++;
        }
    }
    return peaks;
}

frequency_bin* get_pitch(double complex clip[]){
    _convert_to_frequency_domain(clip);
    frequency_bin* peaks = get_peaks(clip);
    
    if(peaks != NULL){
        note_probabilities(peaks, PEAKS_ARR_SIZE, FLOAT_COMPARISON_ERROR);
    }

    return peaks;
}