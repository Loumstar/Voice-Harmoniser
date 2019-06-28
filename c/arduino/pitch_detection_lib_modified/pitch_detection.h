#include <stdbool.h>
#include "fourier_transform.h"
#include "signal_methods.h"
#include "peaks_correlation.h"

/*
The minimum ratio of the amplitude of a frequency, relative
to that of similar frequencies, which would add it to the list of peaks.
*/
#define THRESHOLD 2.5

//The size of the array used to calculate the average amplitude of frequencies
#define SAMPLE_ARR_SIZE 75
//The size of the array containing frequencies identified as peaks.
#define PEAKS_ARR_SIZE 20

//The smallest value for a floating point that is considered to be greater than zero.
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

double get_noise_level(int f, const complex clip[]){
    /*
    Method to determine the amplitude of frequencies surrounding the amplitude at f, or 'noise'.
    
    If the amplitude at f is significantly larger than the noise, the ratio of the two
    will be larger than the threshold.

    If the amplitude passes the threshold test, it is added to the array of peaks.

    The noise is calculated by summing the amplitudes that are within the set of frequencies 
    surrounding f, and is divided by the number of amplitudes in the set.

    The size of this set is determined by SAMPLE_ARR_SIZE.

    PARAMETERS
    ----------
    f:
    The index value of the frequency that is being tested.

    clip:
    The array containing the amplitudes of all frequencies.
    */
    double sum = 0;
    size_t lb, ub;
    //If the sample indices fall outside the upper bound of clip
    if(f + floor(SAMPLE_ARR_SIZE / 2) > CLIP_FRAMES){
        //use the last set of values that make a full set.
        lb = CLIP_FRAMES - SAMPLE_ARR_SIZE;
        ub = CLIP_FRAMES;
    //If the sample indices fall outside the upper bound of clip 
    } else if(f - floor(SAMPLE_ARR_SIZE / 2) < 0){
        //use the first set of values that make a full set.
        lb = 0;
        ub = SAMPLE_ARR_SIZE;
    } else {
        //else use a set where f is the middle value.
        lb = f - floor(SAMPLE_ARR_SIZE / 2);
        ub = f + floor(SAMPLE_ARR_SIZE / 2);
    }
    for(size_t i = lb; i < ub; i++){
        //sum the values
        sum += cabs(clip[i]);
    }
    //divide by the size of the sample.
    return (double) sum / SAMPLE_ARR_SIZE;
}

void _convert_to_frequency_domain(complex clip[]){
    double offset = mean(clip, CLIP_FRAMES);
    remove_offset(clip, offset);
    fft(clip, CLIP_FRAMES);
}

frequency_bin* get_peaks(const complex clip[]){
    frequency_bin* peaks = malloc(FREQUENCY_BIN_SIZE * PEAKS_ARR_SIZE);
    if(peaks == NULL){
        print_malloc_error(__func__, FREQUENCY_BIN_SIZE * PEAKS_ARR_SIZE);
        return NULL;
    }
    
    double noise, amplitude;
    size_t i = 0;
    for(size_t f = 0; f < floor(CLIP_FRAMES / 2); f++){
        noise = get_noise_level(f, clip);
        amplitude = cabs(clip[f]);
        //printf("%.2f, %.2f, %.2f\n", (double) f * FRAME_RATE / CLIP_FRAMES, amplitude, noise);
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

frequency_bin* get_pitches(complex clip[]){
    _convert_to_frequency_domain(clip);
    frequency_bin* peaks = get_peaks(clip);
    if(peaks != NULL){
        note_probabilities(peaks, PEAKS_ARR_SIZE, FLOAT_EPSILON);
    }
    return peaks;
}

double get_pitch(complex clip[]){
    frequency_bin* notes = get_pitches(clip);
    if(notes == NULL) return 0;
    
    double f;
    double max_p = 0;

    for(size_t i = 0; i < PEAKS_ARR_SIZE; i++){
        if(!isnan(notes[i][2]) && notes[i][2] > max_p){
            f = notes[i][0];
        }
    }

    free(notes);
    return f;
}