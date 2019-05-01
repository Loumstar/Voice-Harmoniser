#include <stdio.h>
#include <math.h>
#include <complex.h>

#define PI 3.14159265358979
 
void _fft(double complex waveform[], double complex spectrum[], int n, int step)
{
	if(step < n){
		_fft(spectrum, waveform, n, step * 2);
		_fft(spectrum + step, waveform + step, n, step * 2);
		
        for (int i = 0; i < n; i += 2 * step){
			double complex t = cexp(-I * PI * i / n) * spectrum[i + step];
			
            waveform[i / 2] = spectrum[i] + t;
			waveform[(i + n)/2] = spectrum[i] - t;
		}
	}
}
 
double complex signalCopy(double complex waveform[], n){
    double complex copy[n];
    
    for(int i=0; i<n; i++){
        copy[i] = waveform[i];
    }
    
    return copy;
}

double complex fft(double complex waveform[], int n)
{
	double complex spectrum[n] = signalCopy(waveform[], n);
	_fft(waveform, spectrum, n, 1);
    
    return waveform;
}