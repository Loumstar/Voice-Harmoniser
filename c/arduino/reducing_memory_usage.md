# Removing unnecessary memory allocations

```
double[2] sample[1024]
double[2] copy[1024]
double[3] peaks[20]
double harmonics[20]
```


At 1024 frames per sample, the original memory allocated is 4 kB per sample.
Therefore 8 kB is allocated to the samples alone.
At 20 frequency bins in peaks and 20 frequencies in harmonics, the total memory allocated becomes ~8.5 kB.


## Removing malloc:
Dynamic allocation should be reduced to avoid memory leaks.
Also, the max memory is when sample and its copy are allocated at the same time.
Therefore using the difference between using static and dynamic allocation is very small.

## Converting double to char/int:
SRAM has 2 kB of memory

As the size of each double is 4 bytes, converting this to a char, which is only 1 byte, significantly reduces the memory required.

```
char[2] sample[1024]
char[2] copy[1024]
double[3] peaks[20]
int harmonics[20]
```

The new total memory becomes ~2.5 kB if all statically allocated.

## Reducing size of sample array:
### Requirements: 
- Frequency resolution < 5 Hz
- Sample array size must be a power of 2


1024 is too large, therefore length must be 512. The length of the sample must be greater than 0.2 seconds due to frequency resolution.

Sampling rate must be 512 / 0.2 = 2,560 Hz.<br>
Due to Nyquist, the maximum frequency measurable will be half this, which is 1,280 Hz.<br><br>
__Therefore highest note measurable is Eb6 (1244 Hz).__

## Combined Result

```
char[2] sample[512]
char[2] copy[512]
double[3] peaks[20]
int harmonics[20]
```

The effect of all these is that the size of clip will be reduced to 1/8 its original size.
This means the script will run on an Arduino with more than ~1.5 kB of SRAM.