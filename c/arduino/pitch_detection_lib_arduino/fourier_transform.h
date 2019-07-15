#include <stdio.h>
#include <stdlib.h>
#include "lightweight_complex.h"

#define PI M_PI

void print_complex_array(const complex complex_arr[], size_t s){
    for(size_t j = 0; j < s; j++){
        printf("%d + %di\n", creal(complex_arr[j]), cimag(complex_arr[j]));
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
        
        double_complex dc1, dc2;
        complex t;

        for(size_t a = 0; a < n; a += step * 2){   
            dc1[0] = 0; 
            dc1[1] = -PI * a / n;
            
            dc2[0] = (double) copy[a + step][0];
            dc2[1] = (double) copy[a + step][1];
            
            dcexp(dc1, dc1);   
            dcmult(dc1, dc2, dc2);

            t[0] = (int) dc2[0];
            t[1] = (int) dc2[1];

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