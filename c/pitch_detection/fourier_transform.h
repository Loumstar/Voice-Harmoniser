#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <complex.h>

#define PI M_PI
 
void _fft(double complex waveform[], double complex spectrum[], size_t n, int step)
{
	if(step < n){
		_fft(spectrum, waveform, n, step * 2);
		_fft(spectrum + step, waveform + step, n, step * 2);
		
        for (size_t a = 0; a < n; a += 2 * step){
			double complex t = cexp(-I * PI * a / n) * spectrum[a + step];
			
            waveform[a / 2] = spectrum[a] + t;
			waveform[(a + n)/2] = spectrum[a] - t;
		}
	}
}
 
double complex* signalCopy(double complex waveform[], size_t n){
    double complex* copy = malloc(sizeof(double complex) * n);
    for(size_t a = 0; a < n; a++){
        copy[a] = waveform[a];
    }
    return copy;
}

void fft(double complex waveform[], size_t length)
{
	double complex* spectrum = signalCopy(waveform, length);
	_fft(waveform, spectrum, length, 1);
    free(spectrum);
    printf("FFT\n");
}