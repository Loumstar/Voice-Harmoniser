#include "../pitch_detection.h"
#include "unittest.h"

void print_frequency_bins(const frequency_bin bin[], size_t peaks_arr_size){
    for(size_t j = 0; j < peaks_arr_size; j++){    
        if(!isnan(bin[j][0])){
            printf("(%.f Hz, %.2f dB, %.4f)\n", bin[j][0], bin[j][1], bin[j][2]);
        }
    }
}

complex* create_signal(const double a[1][2], double offset, size_t length){
    /*
    Method to create an array containing the amplitudes of a basic waveform with size CLIP_FRAMES,
    similar to how an audio file will be read.
    */

    //Must be complex type as fft will convert signal to a complex number.
    complex* signl = malloc(sizeof(complex) * CLIP_FRAMES);
    //For each frame in the signal
    for(size_t i = 0; i < CLIP_FRAMES; i++){
        int sum = 0;
        //sum up the amplitudes of each sine wave to create the signal by superposition.
        for(size_t j = 0; j < length; j++){
            sum += a[j][1] * sin((double) 2 * PI * a[j][0] * i / FRAME_RATE);
        }
        signl[i][0] = sum + offset;
        signl[i][1] = 0;
    }
    return signl;
}

int main(void){
    //fft method only handles arrays that have a length equal to a power of two.
    if(!is_power_of_two(CLIP_FRAMES)){
        printf(
            "The number of frames in the clip must be a power of two: log2(%i) = %.3f...\n", 
            CLIP_FRAMES, 
            log2(CLIP_FRAMES)
        );
        return 1;
    }

    double frequency_resolution = FRAME_RATE / CLIP_FRAMES;

    //Output basic properties of the transform.
    printf(
        "The maximum frequency measured is %i Hz.\nThe frequency resolution is %.1f Hz.\nThe length of the clip is %.3fs.\n", 
        (int) FRAME_RATE / 2,
        frequency_resolution, 
        (double) CLIP_FRAMES / FRAME_RATE
    );
    
    //example basic waveform.  
    size_t a_size = 1;   
    double a[1][2] = {
        {125, 128} //125 Hz with -20log(8) dB volume
    };

    //create the signal.
    complex* signl = create_signal(a, 0, a_size);
    //handle malloc error from create_signal() method.
    if(signl == NULL) return 1;
    //determine the probability of notes of the signal.
    frequency_bin* notes = get_pitches(signl);
    
    frequency_bin pitch_bin;
    get_pitch_bin(notes, pitch_bin);
    
    double pitch = get_pitch(notes);

    free(signl);
    
    //handle malloc error from the get_pitch() method.
    if(notes == NULL) return 1;
    
    //print the array of notes.
    print_frequency_bins(notes, PEAKS_ARR_SIZE);

    free(notes);

    double volume = pitch_bin[1];

    if(!pitch) return 1;

    printf("Assert that pitch detected is 125 ± 2 Hz\n");
    printf("    %.f\n", pitch);
    if(assert_double_similar(pitch, 125.0, frequency_resolution / 2)){
        printf("    PASS\n\n");
    } else {
        printf("    FAIL\n\n");
        return 1;
    }

    printf("Assert that volume detected is 20log(128/1024) dB = -18.062...\n");
    printf("    %.3f\n", volume);
    if(assert_double_similar(volume, 20 * log10f(128.0 / 1024.0), 1)){
        printf("    PASS\n\n");
    } else {
        printf("    FAIL\n\n");
        return 1;
    }
    
    return 0;
}