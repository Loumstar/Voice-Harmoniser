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

        self.get_harmonics_arr_size = 20
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

    def get_pitch(self, start):
        """
        Method that combines all methods in the class together to determine the pitch of 
        a snippet or 'clip' of audio.
        """
        spectrum, amplitude = self.frequency_spectrum(start)
        peaks = self.get_peaks(spectrum, amplitude)
        
        spikes = self.get_spikes(peaks)

        if self.print:
            self.plot_clip(spectrum, amplitude, spikes)
        
        return self.get_note_probabilities(spikes)

    def get_peaks(self, spectrum, amplitude):
        """
        Method that returns a list of maximums or 'peaks' in the spectrum.
        
        The rate of change of amplitude between neighbouring frequencies is calculated
        and if the gradient goes from negative to positive, a minimum has been reached. 
        
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
                peak = self._get_singular_peak(spectrum[prev_min_i:i], amplitude[prev_min_i:i])
                peaks.append(peak)
                prev_min_i = i

            prev_gradient = new_gradient
            prev_a = a

        return peaks

    def _gradient(self, y0, y1):
        return y1 - y0

    def _get_singular_peak(self, spectrum, amplitudes):
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

    def get_spikes(self, peaks):
        """
        Method that returns the peaks if the ratio between their amplitude and the noise level
        exceeds the threshold which is used to indicate whether it is a spike.

        Return an sorted list of tuples up to a maximum number to reduce workload
        later.
        """
        spikes = []
        average_noise_level = self.get_average_noise_level(peaks)

        for f, a, _ in peaks:
            noise = next(average_noise_level)
            #if the ratio of amplitude to background noise exceeds the threshold,
            #it is likely that the peak is also a spike, and a possible harmonic.
            if a / noise > self.threshold:
                spikes.append(tuple([f, a, noise]))

        spikes.sort(reverse=True, key=lambda x: x[1] / x[2])
        
        if len(spikes) > self.spikes_arr_size:
            del spikes[self.sample_size:]

        return spikes

    def get_average_noise_level(self, peaks):
        lb, ub = 0, self.sample_size
        noise_sum = sum([p[2] for p in peaks[0:self.sample_size]])
        
        for i in range(len(peaks)):
            if i > self.sample_size // 2 and i < len(peaks) - self.sample_size // 2 - 1:
                lb, ub = lb + 1, ub + 1
                old, new = peaks[lb-1][2], peaks[ub][2]
            else:
                old, new = 0, 0

            noise_sum = noise_sum - old + new

            yield noise_sum / self.sample_size

    def get_note_probabilities(self, spikes):
        notes = []
        
        for s in spikes:
            get_harmonics = self.get_harmonics(s[0])
            p = self.test_harmonics(spikes, get_harmonics)
            
            notes.append(tuple([s[0], p]))

        return notes

    def test_harmonics(self, spikes, get_harmonics):
        #correlation is a value that determines how close to the actual peaks a certain 
        #frequency's get_harmonics are.
        correlation = 0
        
        for h in get_harmonics:
            correlation += self.get_correlation(h, spikes)
        
        return correlation / len(spikes)

    def get_correlation(self, f, spikes):
        c = 0
        for s in spikes:
            c += e ** -((2 * (f - s[0]) / self.spacing) ** 2)
        return c

    def get_harmonics(self, f):
        for h in range(self.get_harmonics_arr_size):
            yield f * (h + 2)

    def plot_clip(self, spectrum, amplitude, spikes):
        spikes_f, spikes_a, spikes_n = self.split_and_sort_spike_tuples(spikes)
        spike_distribution = self.get_full_distribution(spectrum, spikes)
        
        plot.plot(
            spectrum, amplitude, 'r',
            spikes_f, spikes_n, 'b',
            spikes_f, spikes_a, 'g^',
            spectrum, spike_distribution, 'g'
        )

        plot.savefig('./frequency_spectrum.pdf')

    def get_full_distribution(self, spectrum, spikes):
        distribution = []
        
        for x in spectrum:
            p = 0
            for f, a, _ in spikes:
                p += a * e ** -((2 * (x-f) / self.spacing) ** 2)
            
            distribution.append(p)

        return distribution

    def split_and_sort_spike_tuples(self, spikes):
        freq, amp, noise = [], [], []
        
        for (f, a, n) in sorted(spikes, reverse=True, key=lambda x: x[0]):
            freq.append(f)
            amp.append(a)
            noise.append(n)

        return freq, amp, noise