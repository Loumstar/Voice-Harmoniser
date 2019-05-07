#include "./pitch_detector.c"

double complex* createSignal(double complex a[], size_t length){
    double complex* signl = malloc(sizeof(double complex) * CLIP_FRAMES);
    for(int i = 0; i < CLIP_FRAMES; i++){
        double sum = 0;
        double t = (double) i / FRAME_RATE;
        for(size_t j = 0; j < length; j++){
            sum += cimag(a[j]) * sin(2 * PI * creal(a[j]) * t);
        }
        signl[i] = sum - 0.5;
    }
    return signl;
}

int main(){
    double complex a[2] = {
        {20, 1},
        {50, 0.5}
    };

    printf("Begin\n");
    size_t a_size = sizeof(a) / sizeof(double complex);
    double complex* signl = createSignal(a, a_size); //allocates the memory

    print_complex_array(signl, (size_t) 10);

    get_pitch(signl);
    free(signl);
    return 0;
}