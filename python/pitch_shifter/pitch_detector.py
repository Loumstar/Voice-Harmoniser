import wave
from numpy import fft as fourier, linspace, absolute, mean, e

import matplotlib.pyplot as plot

class PitchDetector:
    """
    Class to open an audio file and analyse its waveform to determine its
    pitch across a small interval or 'clip'.

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

        #sample size is the set of frequencies near to a peak whose amplitudes
        #are averaged to determine whether that peak is a spike/harmonic, rather
        #than an 'audio sample'.
        self.sample_size = 11
        #the minimum ratio of the amplitude of a peak compared to average amplitude of
        #frequencies near it in the spectrum that would make that peak a spike.
        self.threshold = 2

        #equivalent to the deviation in the distribution. The value determines at what
        #difference from a mean does the distribution approach zero.
        self.spacing = 50

        self.harmonics_arr_size = 20
        self.spikes_arr_size = 20

        self.print = False

    def frequency_spectrum(self, start):
        """
        Method to return the frequency spectrum and corresponding amplitudes.

        start:
        the number of seconds after the start of the track from where analysis 
        begins.
        """
        #total number of frames in each clip
        clip_frames = int(self.frame_rate * self.clip_length)

        #creates frequency domain for fft. There are (frame rate x clip length)
        #number of frequencies, of which the maximum frequency is equal to the
        #frame rate. However these values have been halved due to Nyquist.
        spectrum = [round(n / self.clip_length) for n in range(clip_frames // 2)]

        if start + self.clip_length < self.audio_length:
            #set position to the first frame of the starting point
            self.audio_file.setpos(int(start * self.frame_rate))
            #read frames in the refresh period starting from the marker
            clip = list(self.audio_file.readframes(clip_frames))
            #calculate the amplitude of each frequency using FFT
            amplitude = self._decompose(clip, clip_frames)
            
            return spectrum, amplitude
        
        return None

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
        fft = fourier.fft(list(map(lambda x: (x - offset) / self.bit_depth, clip)))
        #converts to a real number, removing the phase.
        amplitude = list(map(lambda z: 2 * absolute(z) / clip_frames, fft))
        #only return half the list, due to Nyquist. hence amplitudes are multiplied
        #above by two to account for the lost 'energy'.
        return amplitude[:clip_frames // 2]

    def determine_pitch(self, start):
        spectrum, amplitude = self.frequency_spectrum(start)
        peaks = self.determine_peaks(spectrum, amplitude)
        spikes = self.get_peaks_above_threshold(peaks)
        
        note_probability = []
        
        for s in spikes:
            harmonics = self.harmonics(s[0])
            p = self.test_harmonics(spectrum, spikes, harmonics)
            note_probability.append(tuple([s[0], p]))

        if self.print:
            peaks_f = list(map(lambda x: x[0], peaks))
            peaks_a = list(map(lambda x: x[1], peaks))

            spikes_by_frequency = sorted(spikes, reverse=True, key=lambda x: x[0])
            spikes_f = list(map(lambda x: x[0], spikes_by_frequency))
            spikes_a = list(map(lambda x: x[1], spikes_by_frequency))
            spikes_n = list(map(lambda x: x[2], spikes_by_frequency))

            d = self.get_distribution(spectrum, spikes)
            
            plot.plot(
                spectrum, amplitude, 'r',
                peaks_f, peaks_a, 'b*',
                spikes_f, spikes_n, 'b',
                spikes_f, spikes_a, 'g^',
                spectrum, d, 'g'
            )

            plot.show()
        
        return note_probability

    def determine_peaks(self, spectrum, amplitude):
        """
        Method that returns a list of maximums or 'peaks' in the spectrum.
        
        The rate of change of amplitude between neighbouring frequencies is calculated
        and f the gradient goes from negative to positive, a minimum has been reached. 
        
        The largest value between two minima is found and added to the list as a tuple
        of its frequency, amplitude, and the average amplitude between the two minimas.
        """

        peaks = []
        prev_gradient = 0
        prev_min_i = 0
        prev_a = 0

        for i, a in enumerate(amplitude):
            new_gradient = self._gradient(prev_a, a)

            if prev_min_i != i and new_gradient > 0 and prev_gradient <= 0:
                peak = self._find_peak(spectrum[prev_min_i:i], amplitude[prev_min_i:i])
                peaks.append(peak)
                prev_min_i = i

            prev_gradient = new_gradient
            prev_a = a

        return peaks

    def _gradient(self, y0, y1):
        return y1 - y0

    def _find_peak(self, spectrum, amplitudes):
        """
        Method to find the maximum amplitude in a range of frequencies, or a 'peak'.
        Combines each peak with its frequency and the average amplitude between each 
        minima.
        """
        f, a_max, a_sum = None, 0, 0
        for i, a in enumerate(amplitudes):
            if a_max < a or f == None:
                a_max = a
                f = spectrum[i]
            
            a_sum += a

        a_mean = a_sum / len(amplitudes)

        return f, a_max, a_mean

    def get_peaks_above_threshold(self, peaks):
        """
        Method that returns the peaks if the ratio between their amplitude and the noise level
        exceeds the threshold which is used to indicate whether it is a spike.

        Return an sorted list of tuples up to a maximum number to reduce workload
        later.
        """
        spikes = []
        #as an equal number of frequencies should included in the mean calculation either
        #side of the peak, number is made odd to include peak frequency.
        lb, ub = 0, self.sample_size
        noise = sum([p[2] for p in peaks[lb:ub]]) / self.sample_size

        for i, (f, a, _) in enumerate(peaks):
            #as consecutive averages contain some of the same data points, as opposed to
            #manipulating the same values, only the values to be removed/added are manipulated.
            noise = self._moving_average(noise, peaks, lb, ub)
            #if the ratio of amplitude to background noise exceeds the threshold,
            #it is likely that the peak is also a spike, and a possible harmonic.
            if a / noise > self.threshold:
                spikes.append(tuple([f, a, noise]))
            #bounds for the calculated mean move with each peak.
            if i > self.sample_size // 2:
                lb, ub = lb + 1, ub + 1

        n = len(spikes) if len(spikes) < self.spikes_arr_size else self.spikes_arr_size

        return sorted(spikes, reverse=True, key=lambda x: x[1] / x[2])[:n]

    def _moving_average(self, noise, peaks, lb, ub):
        """
        Method to return the average noise quicker than summing elements in a list.
        Because all values except the lowest value are part of the average, it is quicker
        to simply remove the last value, add the new one and divided by the size of the 
        dataset.
        """
        old_value = peaks[lb - 1][2] if lb > 0 else 0
        new_value = peaks[ub][2] if ub < len(peaks) else 0

        new_noise = (self.sample_size * noise) - old_value + new_value

        return new_noise / self.sample_size

    def test_harmonics(self, spectrum, spikes, harmonics):
        #correlation is a value that determines how close to the actual peaks a certain 
        #frequency's harmonics are.
        correlation = 0
        for h in harmonics:
            correlation += self.correlate(h, spikes)
        return correlation / len(spikes)

    def correlate(self, f, spikes):
        c = 0
        for s in spikes:
            c += e ** -((2 * (f - s[0]) / self.spacing) ** 2)
        return c

    def harmonics(self, f):
        for h in range(self.harmonics_arr_size):
            yield f * (h + 2)

    def get_distribution(self, spectrum, spikes):
        distribution = []
        
        for x in spectrum:
            p = 0
            for f, a, _ in spikes:
                p += a * e ** -((2 * (x-f) / self.spacing) ** 2)
            
            distribution.append(p)

        return distribution