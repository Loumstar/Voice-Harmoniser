#include "../pitch_detection.h"

void print_frequency_bins(const frequency_bin bin[], size_t peaks_arr_size){
    for(size_t j = 0; j < peaks_arr_size; j++){    
        if(!isnan(bin[j][0])){
            printf("(%.f Hz, %.2f dB, %.4f)\n", bin[j][0], bin[j][1], bin[j][2]);
        }
    }
}

complex* create_signal(const double a[4][2], double offset, size_t length){
    /*
    Method to create an array containing the amplitude of a basic waveform, similar to how
    an audio file will be read.
    
    PARAMETERS
    ----------
    a: 
    a complex array containing the frequency and amplitude of a given sinusoid as its real 
    and imaginary parts respectively.

    offset:
    double representing the mean amplitude of the oscillation.

    length:
    the size of a[].
    */
   
    //Create a signal using an array of amplitudes that is CLIP_FRAMES long.
    //Must be complex type as fft will convert signal to a complex number.
    complex* signl = malloc(sizeof(complex) * CLIP_FRAMES);
    //For each frame in the signal
    for(size_t i = 0; i < CLIP_FRAMES; i++){
        char sum = 0;
        //sum up the amplitudes of each sine wave to create the signal by superposition.
        for(size_t j = 0; j < length; j++){
            sum += a[j][1] * sin((double) 2 * PI * a[j][0] * i / FRAME_RATE);
        }
        signl[i][0] = sum + offset;
        signl[i][1] = 0;
    }
    //return a pointer to the signal array.
    return signl;
}

int main(){
    //fft method only handles arrays that have a length equal to a power of two.
    if(!is_power_of_two(CLIP_FRAMES)){
        printf(
            "The number of frames in the clip must be a power of two: log2(%i) = %.3f...\n", 
            CLIP_FRAMES, 
            log2(CLIP_FRAMES)
        );
        return 1;
    }
    //Output basic properties of the transform.
    printf(
        "The maximum frequency measured is %i Hz.\nThe frequency resolution is %.1f Hz.\nThe length of the clip is %.3fs.\n", 
        (int) FRAME_RATE / 2,
        (double) FRAME_RATE / CLIP_FRAMES, 
        (double) CLIP_FRAMES / FRAME_RATE
    );
    
    //example basic waveform.     
    double a[4][2] = {
        {100, 128},
        {200, 256},
        {300, 512},
        {400, 120}
    };

    size_t a_size = 4;

    //create the signal.
    complex* signl = create_signal(a, 0, a_size);
    
    //handle malloc error from create_signal() method.
    if(signl == NULL) return 1;
    
    //determine the probability of notes of the signal.
    frequency_bin* notes = get_pitches(signl);
    
    free(signl);
    
    //handle malloc error from the get_pitch() method.
    if(notes == NULL) return 1;
    
    //print the array of notes.
    print_frequency_bins(notes, PEAKS_ARR_SIZE);
    
    free(notes);
    
    return 0;
}