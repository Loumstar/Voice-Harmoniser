#include <stdint.h>
#include <complex.h>
#include <math.h>
#include <time.h>

#include "Pitch-Detection/pitch_detection.h"
#include "Arduino-Libraries/reharmoniser/reharmoniser.h"
#include "Write-WAV-File/wave_file.h"

int main(void){
    char audio_filename[] = "Testing C-note.wav";
    char reharmonised_filename[] = "Testing C-note.reharmonised.wav";

    /*
    The notes to reharmonise the audio_file to.
    To be replaced with a midi message for full integration test.
    */

    note notes[MAX_VOICES] = {
        {40, 261.6, 128},
        {44, 329.6, 128},
        {47, 392.0, 128}
    };

    size_t buffer_size;

    int32_t* frame_array;
    
    int32_t* buffer = NULL;
    double complex* complex_buffer = NULL;

    double buffer_time = pow(10, -3);
    double pitch;

    // Read wave file to audio_file
    Wave audio_file = read_wave_metadata(audio_filename);

    uint32_t sample_rate = audio_file.header.sample_rate;
    uint32_t numberof_samples = audio_file.numberof_samples;
    uint16_t bits_per_sample = audio_file.header.bits_per_sample;

    /*
    Determine size of each buffer.
    This must be a power of two so buffer_time is rounded to the nearest power.
    */

    buffer_size = (size_t) pow(2, round(log2f(buffer_time * sample_rate)));
    
    printf("Audio File Length: %.3f s\n", (double) (numberof_samples / sample_rate));
    printf("Buffer Length: %.3f ms\n", ((double) buffer_size / sample_rate) * 1000);

    /*
    Determine size of the audio_file.

    This must also be a power of two but two avoid cutting part of the audio_file short, 
    the upper power is selected. The extra frames are initialised to zero when allocated.
    */

    size_t frame_size = pow(2, ceil(log2f(numberof_samples)));
    frame_array = malloc(frame_size * sizeof(int32_t));

    if(frame_array){
        // Write the data to the audio_file array
        read_wave_data_to_array(&audio_file, frame_array);
        // Allocate memory for the circular buffer with int32_t type (for reharmonising)
        buffer = malloc(buffer_size * sizeof(int32_t));
        // Allocate memory for the circular buffer with complex type (for pitch detection)
        complex_buffer = malloc(buffer_size * sizeof(double complex));
    }

    if(buffer && complex_buffer){
        // For each buffer
        size_t start = clock();
        for(size_t i = 0; i < frame_size; i += buffer_size){
            
            // Read wave data into buffer
            for(size_t j = 0; j < buffer_size && (i + j) < frame_size; j++){
                buffer[j] = frame_array[i + j];
                complex_buffer[j] = (double) frame_array[i + j];
            }

            // Determine pitch of the buffer
            pitch = get_pitch(complex_buffer, buffer_size, sample_rate, bits_per_sample);            

            // Write the reharmonised version back into the audio_file array
            for(size_t j = 0; j < buffer_size && (i + j) < frame_size; j++){
                frame_array[i + j] = get_reharmonised_sample(buffer, notes, pitch, j, buffer_size);
            }
            
        }
        size_t end = clock();

        printf("Harmoniser conversion took %.3f ms.\n", ((double) end - start) * 1000 / CLOCKS_PER_SEC);

    } else {
        printf("Failed to convert due to malloc error of audio_file or buffer arrays.\n");
    }

    // Create a wave file using the reharmonised data in frame_array
    write_array_to_wav_file(reharmonised_filename, frame_array, frame_size, 1, sample_rate, bits_per_sample);
    
    if(frame_array) free(frame_array);

    if(buffer) free(buffer);
    if(complex_buffer) free(complex_buffer);

    return 0;
}
