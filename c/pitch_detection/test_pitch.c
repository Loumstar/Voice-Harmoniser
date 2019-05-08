#include "./pitch_detection.c"

#define SIGNAL_MALLOC_ERROR NAN + (I * NAN)

double complex* createSignal(double complex a[], double offset, size_t length){
    double complex* signl = malloc(sizeof(double complex) * CLIP_FRAMES);
    if(signl == NULL){
        printf("Malloc error during createSignal() method: failed while requesting %zu bytes.", sizeof(double complex) * CLIP_FRAMES);
        return NULL;
    }
    for(size_t i = 0; i < CLIP_FRAMES; i++){
        double sum = 0;
        double t = (double) i / FRAME_RATE;
        for(size_t j = 0; j < length; j++){
            sum += cimag(a[j]) * sin((double) 2 * PI * creal(a[j]) * i / FRAME_RATE);
        }
        signl[i] = sum + offset;
    }
    return signl;
}

int main(){
    int exit_code = 0;
    
    double complex a[2] = {
        {20, 1},
        {50, 0.5}
    };
    
    size_t a_size = sizeof(a) / sizeof(double complex);
    
    double complex* signl = createSignal(a, -0.5, a_size); //allocates the memory
    
    if(signl == NULL) return 1;
    frequency_bin* notes = get_pitch(signl);
    
    free(signl);
    
    if(notes == NULL) return 1;
    print_frequency_bins(notes, PEAKS_ARR_SIZE);
    free(notes);
    return 0;

}