#include <stdio.h>
#include <stdlib.h>
#include "complex_mod.h"

#define PI M_PI

void print_complex_array(const complex complex_arr[], size_t s){
    for(size_t j = 0; j < s; j++){
        printf("%.2f + i%.2f\n", creal(complex_arr[j]), cimag(complex_arr[j]));
    }
}

void print_magnitude(const complex complex_arr[], size_t s){
    for(size_t j = 0; j < s; j++){
        printf("%lu Hz: %.2f\n", j, cabs(complex_arr[j]));
    }
}

complex* copy_signal(complex waveform[], size_t n){
    complex* copy = malloc(sizeof(complex) * n);
    for(size_t a = 0; a < n; a++){
        copy[a][0] = creal(waveform[a]);
        copy[a][1] = cimag(waveform[a]);
    }
    return copy;
}
 
void _fft(complex waveform[], complex copy[], size_t n, size_t step){
    if(step < n){
        _fft(copy, waveform, n, step * 2);
        _fft(copy + step, waveform + step, n, step * 2);
        complex c, t;

        for (size_t a = 0; a < n; a += step * 2){   
            c[0] = 0; c[1] = -PI * a / n;
            cexp(c, c);   
            cmult(c, copy[a + step], t);
            cadd(copy[a], t, waveform[a / 2]);
            csub(copy[a], t, waveform[(a + n) / 2]);
        }
    }
}

void fft(complex waveform[], size_t length){
    complex* copy = copy_signal(waveform, length);
    _fft(waveform, copy, length, 1);
    free(copy);
}