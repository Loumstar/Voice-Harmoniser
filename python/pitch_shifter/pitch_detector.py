import wave
from numpy import fft as fourier, absolute, mean, e

import matplotlib.pyplot as plot

class PitchDetector:
    """
    Class to open an audio file and analyse its waveform to determine its
    pitch across a small interval or 'clip'.

    Parameters
    ----------
    audio_filename: 
    A string of the path to the target audio file. Can be absolute or relative.
    """
    def __init__(self, audio_filename):
        #opens the standard file. Must be wav format.
        self.audio_file = wave.open(audio_filename)
        #number of frames in the waveform captured per second. Typically 44.1 kHz
        self.frame_rate = self.audio_file.getframerate()
        #number of bits used to define the waveform
        self.bit_depth = 16 ** self.audio_file.getsampwidth()
        #the length of the file in seconds.
        self.audio_length = self.audio_file.getnframes() / self.frame_rate
        
        #the length in seconds of each section of audio that undergoes fft at once.
        #smaller lengths return a smaller set of frequencies, but reduce workload.
        self.clip_length = 0.2

        #the minimum ratio of the amplitude of a frequency compared to average amplitude of
        #frequencies near it in the spectrum that would make it a peak.
        self.threshold = 3
        #the minimum probability that a note representing the frequency spectrum can have and
        #be added to the list of probabilities.
        self.probability_threshold = 10 ** -3

        #equivalent to the deviation in the distribution. The value determines at what
        #difference from a mean does the distribution approach zero.
        self.spacing = 50

        self.harmonics_arr_size = 20
        self.peaks_arr_size = 20

        self.sample_arr_size = 75

        self.print = False

    def frequency_spectrum(self, start):
        """
        Method to return the frequency spectrum and corresponding amplitudes.
        
        Parameters
        ----------
        start:
        the number of seconds after the start of the track from where analysis 
        begins.
        """
        #total number of frames in each clip
        clip_frames = int(self.frame_rate * self.clip_length)

        #creates frequency domain for fft. There are (frame rate x clip length)
        #number of frequencies, of which the maximum frequency is equal to the
        #frame rate. However these values have been halved due to Nyquist.
        spectrum = tuple(round(n / self.clip_length) for n in range(clip_frames // 2))

        if start + self.clip_length < self.audio_length:
            #set position to the first frame of the starting point
            self.audio_file.setpos(int(start * self.frame_rate))
            #read frames in the refresh period starting from the marker
            clip = tuple(self.audio_file.readframes(clip_frames))
            #calculate the amplitude of each frequency using FFT
            amplitude = self._decompose(clip, clip_frames)
            
            return spectrum, amplitude
        
        raise ValueError("The clip being analysed goes beyond the end of the audio file.")

    def _decompose(self, clip, clip_frames):        
        """
        Method to convert waveform into a frequency spectrum, using fast-fourier transform
        (FFT) to determine the amplitude and phase of each frequency, given as a complex 
        number. Phase is not needed so only the magnitude of the number is returned.
        
        the offset amplitude of the waveform is the average amplitude of each frame.
        This is subtracted from each to avoid a large amplitude at the 0Hz frequency.

        The value of x needs to be within 0 and 1, so x is divided by the audio bit depth.

        Each amplitude is divided by the number of frames used in the fft and multiplied by
        two to account for 'mirrored frequencies' that were removed (see Nyquist Frequency).
        """
        #offset is equal to the average amplitude a frame.
        offset = mean(clip)
        #apply fast fourier transform. This is the line that does the legwork.
        fft = fourier.fft(tuple(map(lambda x: (x - offset) / self.bit_depth, clip)))
        #converts to a real number, removing the phase.
        amplitude = tuple(map(lambda z: 2 * absolute(z) / clip_frames, fft))
        #only return half the list, due to Nyquist. hence amplitudes are multiplied
        #above by two to account for the lost 'energy'.
        return amplitude[:clip_frames // 2]

    def get_pitch(self, start):
        """
        Method that returns a list of frequencies whose harmonics represent the frequency
        spectrum of a clip the best.

        spectrum is a list of all the frequencies calculated using FFT.
        
        amplitudes is a list of the amplitudes corresponding to the frequencies in 
        the variable, spectrum.

        peaks are the singular amplitudes that have a significantly larger amplitude
        compared to its surrounding frequencies, leading to the assumption they are
        harmonics.

        Parameters
        ----------
        start:
        A float value of the time into the audio, in seconds, at which the analuysis 
        begins.
        """
        spectrum, amplitude = self.frequency_spectrum(start)
        peaks = self.get_peaks(spectrum, amplitude)

        if self.print:
            self.plot_clip(spectrum, amplitude, peaks)
        
        return self.get_note_probabilities(peaks)

    def get_peaks(self, spectrum, amplitude):
        """
        Method to determine the peaks in a frequency spectrum. These are determined by 
        comparing their amplitudes to the average amplitude of surrounding frequencies.

        If this ratio is above the threshold, they are added to a list and returned.

        Parameters
        ----------
        spectrum:
        The range of frequencies found using FFT.

        amplitude:
        The amplitudes corresponding to each frequency in spectrum.
        """
        peaks = []
        average_noise_level = self._get_average_noise_level(amplitude)

        for i, a in enumerate(amplitude):
            noise = next(average_noise_level)

            if a / noise > self.threshold and self._is_max(amplitude[i-1], a, amplitude[i+1]):
                peaks.append(tuple([spectrum[i], a, noise]))

        peaks.sort(reverse=True, key=lambda x: x[1] / x[2])
        
        if len(peaks) > self.peaks_arr_size:
            del peaks[self.sample_arr_size:]

        return peaks

    def _get_average_noise_level(self, amplitude):
        """
        Method that returns a generator, which is used to calculate the average amplitude
        of surrounding frequencies. 
        
        The range of freequencies used in the calculation is given by sample_arr_size.

        As the calculate moves through the spectrum, it is more efficient to keep track of
        a sum of the amplitudes, removing those that leave the sample while adding those
        enter, reducing computing workload.

        Parameters
        ----------
        amplitude:
        The list of ampltiudes of each frequency, in order of increasing frequency.
        """
        lb, ub = 0, self.sample_arr_size
        noise_sum = sum(amplitude[0:self.sample_arr_size])
        
        for i in range(len(amplitude)):
            if i > (self.sample_arr_size // 2) and i < len(amplitude) - (self.sample_arr_size // 2) - 1:
                lb, ub = lb + 1, ub + 1
                old, new = amplitude[lb-1], amplitude[ub]
            else:
                old, new = 0, 0

            noise_sum = noise_sum - old + new

            yield noise_sum / self.sample_arr_size

    def _is_max(self, y0, y1, y2):
        """
        Method that determines if y1 is a maximum.
        Assertion is made by checking the gradient before y1 is positive and the gradient
        after is negative.

        Parameters
        ----------
        y0:
        The previous y value.

        y1:
        The y value being tested.

        y2:
        The next y value.
        """
        return True if (y1 - y0 > 0) and (y2 - y1 < 0) else False

    def get_note_probabilities(self, peaks):
        """
        Method that returns a list of tuples containing a note and a value corresponding
        to their 'goodness of fit' to the list of peaks.

        If the value is 1, the frequency represents the spectrum exactly, else value is
        between 0 and 1.

        Parameters
        ----------
        peaks:
        A list of tuples containing the frequency, amplitude and surrounding noise
        of each frequency identified as a peak.
        """
        notes = []
        
        for p in peaks:
            harmonics = self.get_harmonics(p[0])
            probability = self.test_harmonics(peaks, harmonics)
            
            if probability > self.probability_threshold:
                notes.append(tuple([p[0], probability]))

        return notes

    def test_harmonics(self, peaks, harmonics):
        """
        Method to test an individual note by comparing each of its harmonics to the
        distribution of peaks in the frequency spectrum.

        Parameters
        ----------
        peaks:
        A list of tuples containing the frequency, amplitude and surrounding noise
        of each frequency identified as a peak.

        harmonics:
        A list of frequencies that are harmonics of a given fundamental frequency.
        """
        correlation = 0
        
        for h in harmonics:
            correlation += self.get_correlation(h, peaks)
        
        return correlation / len(peaks)

    def get_correlation(self, f, peaks):
        """
        Method that returns the goodness of fit of a singular harmonic to any of the peaks.
        This is done using a variation on the equation for the bell curve.

        Parameters
        ----------
        f:
        The value of the frequency being tested. Must be float or int.

        peaks:
        A list of tuples containing the frequency, amplitude and surrounding noise
        of each frequency identified as a peak.
        """
        c = 0
        for s in peaks:
            c += e ** -((2 * (f - s[0]) / self.spacing) ** 2)
        
        return c

    def get_harmonics(self, f):
        """
        Method that returns a generator of the harmonics of a given frequency.

        Parameters
        ----------
        f:
        The fundamental frequency that the harmonics are based off of.
        """
        for h in range(self.harmonics_arr_size):
            yield f * (h + 1)

    def plot_clip(self, spectrum, amplitude, peaks):
        """
        Method to plot the frequemcy spectrum including the variation in noise and
        the distribution of peaks.

        Parameters
        ---------
        spectrum:
        The range of frequencies found using FFT.

        amplitude:
        The amplitudes corresponding to each frequency in spectrum.

        peaks:
        A list of tuples containing the frequency, amplitude and surrounding noise
        of each frequency identified as a peak.
        """
        freq, amp, noise = zip(*sorted(peaks, reverse=True, key=lambda x: x[0]))
        probability_distribution = self.get_full_distribution(spectrum, peaks)
        
        plot.plot(
            spectrum, amplitude, 'r',
            freq, noise, 'b',
            freq, amp, 'g^',
            spectrum, probability_distribution, 'g'
        )

        plot.savefig('./frequency_spectrum.pdf')
        plot.show()

    def get_full_distribution(self, spectrum, peaks):
        """
        Method to return a graph of the bell curves for each peak.
        Not exactly equal to the correlation method, but helps explain how
        the correlation method works
        
        Parameters
        ----------
        spectrum:
        The range of frequencies found using FFT.

        amplitude:
        The amplitudes corresponding to each frequency in spectrum.

        peaks:
        A list of tuples containing the frequency, amplitude and surrounding noise
        of each frequency identified as a peak.
        """
        distribution = []
        
        for x in spectrum:
            max_y, y = 0, 0
            
            for f, a, _ in peaks:
                y = a * e ** -((2 * (x-f) / self.spacing) ** 2)
                max_y = y if max_y < y else max_y
            
            distribution.append(max_y)

        return distribution