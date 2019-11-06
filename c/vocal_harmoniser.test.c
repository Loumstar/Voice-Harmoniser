#include <stdint.h>
#include <complex.h>

#include "pitch_detection.h"
#include "reharmoniser.h"
#include "wave_file.h"

int main(void){
    char sample_filename[] = "elements_vocals_sample.wav";
    char reharmonised_filename[] = "elements_vocals_reharmonised.wav";

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

    sample_buffer_size = (size_t) sample.header.sample_rate * sample_buffer_time;

    // Allocate memory for the sample array
    sample_array = malloc(sample.numberof_samples * sizeof(int32_t));
    // Write the data to the sample array
    read_wave_data_to_array(&sample, sample_array);

    // Allocate memory for the circular buffer with int32_t type (for reharmonising)
    buffer = malloc(sample_buffer_size * sizeof(int32_t));
    // Allocate memory for the circular buffer with complex type (for pitch detection)
    complex_buffer = malloc(sample_buffer_size * sizeof(double complex));

    for(size_t i = 0; i < sample.numberof_samples; i += sample_buffer_size){
        for(size_t j = 0; j < sample_buffer_size && (i + j) < sample.numberof_samples; j++){
            buffer[j] = sample_array[i + j];
            complex_buffer[j] = (double) (sample_array[i + j] * PD_SAMPLE_BIT_DEPTH / INT32_MAX);
        }

        pitch = get_pitch(complex_buffer);            
        
        for(size_t j = 0; j < sample_buffer_size && (i + j) < sample.numberof_samples; j++){
            sample_array[i + j] = get_reharmonised_wave_amplitude(buffer, notes, pitch, j, sample_buffer_size);
        }
    }

    write_array_to_wav_file(reharmonised_filename, sample_array, sample.numberof_samples, 1, sample.header.sample_rate, sample.header.bits_per_sample);

    free(complex_buffer);
    free(sample_array);
    free(buffer);

    return 0;
}
