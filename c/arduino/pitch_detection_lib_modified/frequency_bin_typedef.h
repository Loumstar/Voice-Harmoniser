#define FREQUENCY_BIN_SIZE sizeof(double) * 3
typedef double frequency_bin[3];

void print_frequency_bins(frequency_bin bin[], size_t peaks_arr_size){
    for(size_t j = 0; j < peaks_arr_size; j++){    
        if(!isnan(bin[j][0])){
            printf("(%.f Hz, %.2f dB, %.4f)\n", bin[j][0], bin[j][1], bin[j][2]);
        }
    }
}

void print_malloc_error(const char* function_name, size_t malloc_size){
    printf(
            "Malloc error at %s method: failed to allocate %zu bytes.\n",
            function_name,
            malloc_size
        );
}