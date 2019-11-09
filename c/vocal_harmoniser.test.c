#include <stdint.h>
#include <complex.h>
#include <math.h>
#include <time.h>

#include "Pitch-Detection/pitch_detection.h"
#include "Arduino-Libraries/reharmoniser/reharmoniser.h"
#include "Write-WAV-File/wave_file.h"

int main(void){
    char sample_filename[] = "Testing C-note.wav";
    char reharmonised_filename[] = "note_reharmonised_to_chord.wav";

    /*
    The notes to reharmonise the sample to.
    To be replaced with a midi message for full integration test.
    */

    note notes[MAX_VOICES] = {
        {40, 261.6, 128},
        {44, 329.6, 128},
        {47, 392.0, 128}
    };

    size_t sample_buffer_size;

    int32_t* sample_array;
    int32_t* buffer;

    double complex* complex_buffer;

    double sample_buffer_time = pow(10, -3);
    double pitch;

    // Read wave file to sample
    Wave sample = read_wave(sample_filename);

    /*
    Determine size of each buffer.
    This must be a power of two so sample_buffer_time is rounded to the nearest power.
    */

    sample_buffer_size = (size_t) pow(2, round( log2f(sample_buffer_time * sample.header.sample_rate) ));
    
    printf("Sample Length: %.3f s\n", (double) (sample.numberof_samples / sample.header.sample_rate));
    printf("Sample Buffer Length: %.3f ms\n", ((double) sample_buffer_size / sample.header.sample_rate) * 1000);

    /*
    Determine size of the sample.

    This must also be a power of two but two avoid cutting part of the sample short, 
    the upper power is selected. The extra frames are initialised to zero when allocated.
    */

    size_t sample_size = pow(2, ceil(log2f(sample.numberof_samples)));
    sample_array = malloc(sample_size * sizeof(int32_t));

    if(sample_array){
        // Write the data to the sample array
        read_wave_data_to_array(&sample, sample_array);
        // Allocate memory for the circular buffer with int32_t type (for reharmonising)
        buffer = malloc(sample_buffer_size * sizeof(int32_t));
        // Allocate memory for the circular buffer with complex type (for pitch detection)
        complex_buffer = malloc(sample_buffer_size * sizeof(double complex));
    }

    if(buffer && complex_buffer){
        // For each buffer
        size_t start = clock();
        for(size_t i = 0; i < sample_size; i += sample_buffer_size){
            
            // Read wave data into buffer
            for(size_t j = 0; j < sample_buffer_size && (i + j) < sample_size; j++){
                buffer[j] = sample_array[i + j];
                complex_buffer[j] = (double) sample_array[i + j];
            }

            // Determine pitch of the buffer
            pitch = get_pitch(complex_buffer, sample_buffer_size, sample.header.sample_rate, sample.header.bits_per_sample);            

            // Write the reharmonised version back into the sample array
            for(size_t j = 0; j < sample_buffer_size && (i + j) < sample_size; j++){
                sample_array[i + j] = get_reharmonised_wave_amplitude(buffer, notes, pitch, j, sample_buffer_size);
            }
            
        }
        size_t end = clock();

        printf("Harmoniser conversion took %.3f ms.\n", ((double) end - start) * 1000 / CLOCKS_PER_SEC);

    } else {
        printf("Failed to convert due to malloc error of sample or buffer arrays.\n");
    }

    // Create a wave file using the reharmonised data in sample_array
    write_array_to_wav_file(reharmonised_filename, sample_array, sample_size, 1, sample.header.sample_rate, sample.header.bits_per_sample);
    
    if(sample_array) free(sample_array);

    if(buffer) free(buffer);
    if(complex_buffer) free(complex_buffer);

    return 0;
}
