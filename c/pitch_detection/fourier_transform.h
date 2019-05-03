#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <stdlib.h>

#define PI 3.14159265358979
 
void _fft(double complex waveform[], double complex spectrum[], int n, int step)
{
	if(step < n){
		_fft(spectrum, waveform, n, step * 2);
		_fft(spectrum + step, waveform + step, n, step * 2);
		
        for (int a = 0; a < n; a += 2 * step){
			double complex t = cexp(-I * PI * a / n) * spectrum[a + step];
			
            waveform[a / 2] = spectrum[a] + t;
			waveform[(a + n)/2] = spectrum[a] - t;
		}
	}
}
 
double complex* signalCopy(double complex waveform[], int n){
    double complex* copy = malloc(sizeof(double complex) * n);
    for(int a = 0; a < n; a++){
        copy[a] = waveform[a];
    }
    return copy;
}

double complex fft(double complex waveform[])
{
	int n = sizeof(*waveform)/sizeof(double complex);
	double complex* spectrum = signalCopy(waveform, n);
	_fft(waveform, spectrum, n, 1);
    
    return *waveform;
}

int main(){
	return 0;
}