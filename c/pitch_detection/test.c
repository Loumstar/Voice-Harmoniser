#include "pitch_detection.h"

bool is_power_of_two(int x)
{
    return (x & (x - 1)) == 0;
}

double complex* create_signal(double complex a[], double offset, size_t length){
    /*
    Method to create an array containing the amplitude of a basic waveform, similar to how
    an audio file will be read.
    
    PARAMETERS
    ----------
    a: 
    a complex array containing the frequency and amplitude of a given sinusoid as its real 
    and imaginary parts respectively.

    offset:
    double represrnting the mean amplitude of the oscillation.

    length:
    the size of a[].
    */
    double complex* signl = malloc(sizeof(double complex) * CLIP_FRAMES);
    if(signl == NULL){
        printf(
            "Malloc error during create_signal() method: failed to allocate %zu bytes.", 
            sizeof(double complex) * CLIP_FRAMES
        );
        return NULL;
    }
    for(size_t i = 0; i < CLIP_FRAMES; i++){
        double sum = 0;
        for(size_t j = 0; j < length; j++){
            sum += cimag(a[j]) * sin((double) 2 * PI * creal(a[j]) * i / FRAME_RATE);
        }
        signl[i] = sum + offset;
    }
    return signl;
}

int main(){
    if(!is_power_of_two(CLIP_FRAMES)){
        printf("The number of frames in the clip must be a power of two: log2(%i) = %.3f...\n", CLIP_FRAMES, log2(CLIP_FRAMES));
        return 1;
    }

    printf(
        "The maximum frequency measured is %i Hz.\nThe frequency resolution is %.1f Hz.\nThe length of the clip is %.1fs.\n", 
        (int) FRAME_RATE / 2,
        (double) FRAME_RATE / CLIP_FRAMES, 
        (double) CLIP_FRAMES / FRAME_RATE
    );
    //example basic waveform. 
    double complex a[3] = {
        {20, 255},
        {50, 128},
        {75.625, 120}
    };
    size_t a_size = sizeof(a) / sizeof(double complex);
    //create the signal.
    double complex* signl = create_signal(a, -0.5, a_size);
    
    if(signl == NULL) return 1;
    //determine the probability of notes of the signal.
    frequency_bin* notes = get_pitch(signl);
    free(signl);
    
    if(notes == NULL) return 1;
    //print the array of notes.
    print_frequency_bins(notes, PEAKS_ARR_SIZE);
    free(notes);
    
    return 0;
}