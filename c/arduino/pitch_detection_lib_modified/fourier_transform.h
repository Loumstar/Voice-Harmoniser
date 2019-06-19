#include <stdlib.h>
#include <stdio.h>
#include "complex_mod.h"

#define PI M_PI

void print_complex_array(complex complex_arr[], size_t s){
    for(size_t j = 0; j < s; j++){
        printf("%.2f + i%.2f\n", creal(&complex_arr[j]), cimag(&complex_arr[j]));
    }
}

void print_magnitude(complex complex_arr[], size_t s){
    for(size_t j = 0; j < s; j++){
        printf("%lu Hz: %.2f\n", j, cabs(&complex_arr[j]));
    }
}

complex* copy_signal(complex waveform[], size_t n){
    complex* copy = malloc(sizeof(complex) * n);
    for(size_t a = 0; a < n; a++){
        copy[a] = waveform[a];
    }
    return copy;
}
 
void _fft(complex waveform[], complex spectrum[], size_t n, size_t step){
    if(step < n){
        _fft(spectrum, waveform, n, step * 2);
        _fft(spectrum + step, waveform + step, n, step * 2);
        complex c, t;

        for (size_t a = 0; a < n; a += 2 * step){   
            c[1] = -PI * a / n;         
            t = *cmult(cexp(&c), &spectrum[a + step]);

            waveform[a / 2] = cadd(&spectrum[a], &t);
            waveform[(a + n) / 2] = csub(&spectrum[a], &t);
        }
    }
}

void fft(complex waveform[], size_t length){
    complex* spectrum = copy_signal(waveform, length);
    _fft(waveform, spectrum, length, 1);

    free(spectrum);
}