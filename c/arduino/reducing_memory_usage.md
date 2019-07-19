# Reducing the memory allocated in the Vocal Harmoniser
## Introduction
```
double[2] sample[1024]
double[2] copy[1024]
double[3] peaks[20]
double harmonics[20]
```

The size of the SRAM on an Arduino Uno is 2 kB.

If necessary, this can be upgraded to 8kB with the Arduino ATmega2560 (which might be cheaper as both arduino scripts could be run simultaneously using multithreading).

At 1024 frames per sample, the original memory allocated is 8 kB per sample.
Therefore 16 kB is allocated to the samples alone.
At 20 frequency bins in peaks and 20 frequencies in harmonics, the total memory allocated becomes ~16.5 kB.


## Removing malloc
Dynamic allocation should be reduced to avoid memory leaks.
Also, the max memory is when sample and its copy are allocated at the same time.
Therefore using the difference between using static and dynamic allocation is very small.

## Converting sample from double to int datatype

As the size of each double is 4 bytes, converting this to a int, which is only 2 bytes, significantly reduces the memory required.

```
int[2] sample[1024]
int[2] copy[1024]
double[3] peaks[20]
int harmonics[20]
```

The new total memory becomes ~8.5 kB if all statically allocated.

## Reducing size of sample array
### Requirements: 
- Frequency resolution < 5 Hz
- Sample array size must be a power of 2

### N = 512
Sampling rate will be 512 / 0.2 = 2,560 Hz.<br>
Due to Nyquist, the maximum frequency measurable will be half this, which is 1,280 Hz.<br><br>
__Therefore highest note measurable is Eb6 (1244 Hz).__

### N = 1024
Sampling rate will be 1024 / 0.2 = 5,120 Hz.<br>
Due to Nyquist, the maximum frequency measurable will be half this, which is 2,560 Hz.<br><br>
__Therefore highest note measurable is Eb7 (2489 Hz).__