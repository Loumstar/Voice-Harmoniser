#define FREQUENCY_BIN_SIZE sizeof(double) * 3
typedef double frequency_bin[3];

void print_frequency_bins(frequency_bin bin[], size_t peaks_arr_size){
    for(size_t j = 0; j < peaks_arr_size; j++){    
        printf("(%.0f Hz, %.2f dB, %.4f)\n", bin[j][0], bin[j][1], bin[j][2]);
    }
}