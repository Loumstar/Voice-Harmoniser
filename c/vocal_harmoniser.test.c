#include <stdint.h>
#include <complex.h>
#include <math.h>

#include "pitch_detection.h"
#include "reharmoniser.h"
#include "wave_file.h"

int main(void){
    char sample_filename[] = "Testing C-note.wav";
    char reharmonised_filename[] = "note_reharmonised_to_chord.wav";

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

    sample_buffer_size = (size_t) pow(2, round(log2f(sample.header.sample_rate * sample_buffer_time)));

    printf("Sample Buffer Length: %.2f ms\n", ((float) sample_buffer_size / sample.header.sample_rate) * 1000);

    size_t sample_size = pow(2, ceil(log2f(sample.numberof_samples)));

    // Allocate memory for the sample array
    sample_array = malloc(sample_size * sizeof(int32_t));
    // Write the data to the sample array
    read_wave_data_to_array(&sample, sample_array);

    // Allocate memory for the circular buffer with int32_t type (for reharmonising)
    buffer = malloc(sample_buffer_size * sizeof(int32_t));
    // Allocate memory for the circular buffer with complex type (for pitch detection)
    complex_buffer = malloc(sample_buffer_size * sizeof(double complex));

    for(size_t i = 0; i < sample_size; i += sample_buffer_size){
        for(size_t j = 0; j < sample_buffer_size && (i + j) < sample_size; j++){
            buffer[j] = sample_array[i + j];
            complex_buffer[j] = (double) sample_array[i + j];
        }

        pitch = get_pitch(complex_buffer, sample_buffer_size, sample.header.sample_rate, sample.header.bits_per_sample);            

        for(size_t j = 0; j < sample_buffer_size && (i + j) < sample_size; j++){
            sample_array[i + j] = get_reharmonised_wave_amplitude(buffer, notes, pitch, j, sample_buffer_size);
        }
    }

    write_array_to_wav_file(reharmonised_filename, sample_array, sample_size, 1, sample.header.sample_rate, sample.header.bits_per_sample);
    
    free(complex_buffer);
    free(sample_array);
    free(buffer);

    return 0;
}
