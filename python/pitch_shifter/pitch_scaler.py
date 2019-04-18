import wave
from numpy import fft as fourier, linspace, absolute, sin, pi, mean, e
import matplotlib.pyplot as plot

class PitchScaler:
    def __init__(self, audio_filename):
        self.audio_file = wave.open(audio_filename)
        self.frame_rate = self.audio_file.getframerate() #in samples per second
        self.bit_depth = 16 ** self.audio_file.getsampwidth()
        self.audio_length = self.audio_file.getnframes() / self.frame_rate

    def audio_spectrum(self, start, clip_length):
        #total number of samples taken for a given
        clip_frames = int(self.frame_rate * clip_length)

        #create x axis, which can calculate the existing frequencies up to half 
        #the sampling frequency, due to Nyquist frequency theorem 
        spectrum = linspace(0, self.frame_rate // 2, num=clip_frames // 2)
        
        #for the moment, will only create a spectrum for a clip taken {start} 
        #seconds from first byte for {clip_length} number of seconds
        
        if start + clip_length < self.audio_length:
            self.audio_file.setpos(int(start * self.frame_rate)) #set position to the first frame of the starting second
            clip = list(self.audio_file.readframes(clip_frames)) #read frames in the refresh period starting from the marker
            
            amplitude = self._decompose(clip, clip_frames)
            
            return spectrum, amplitude
        
        return None

    def _decompose(self, clip, clip_frames):        
        #apply fast-fourier-transform to determine the amplitude and phase of each frequency, given as a complex number.
        #as x ranges from 0 to 255, the average x will be non zero, giving the whole clip an offset amplitude.
        #the value of x needs to be within 0 and 1, so x is divided by the audio bit depth
        offset = mean(clip) / self.bit_depth
        fft = fourier.fft(list(map(lambda x: (x / self.bit_depth) - offset, clip)))

        #phase of each frequency is disregarded by finding the magnitude of the complex number
        #each frequency is divided by the number of sample frames and multiplied by two to account
        #for removing the mirrored frequencies due to Nyquist frequency theory.
        amplitude = list(map(lambda x: 2 * absolute(x) / clip_frames, fft))

        return amplitude[:clip_frames // 2]

    def determine_pitch(self, start, clip_length):
        spectrum, amplitude = self.audio_spectrum(start, clip_length)
        peaks = self.determine_peaks(spectrum, amplitude) #sorted by frequency ascending
        
        spikes = self.determine_spikes(peaks, 10, 3)

        peaks_f = list(map(lambda x: x[0], peaks))
        peaks_a = list(map(lambda x: x[1], peaks))

        spikes_f = list(map(lambda x: x[0], spikes))
        spikes_a = list(map(lambda x: x[1], spikes))
        spikes_n = list(map(lambda x: x[2], spikes))

        spikes_distr = self.normal_d_spikes(spectrum, spikes, 50)
        note_probability = []
        
        for s in spikes:
            harmonics = self.harmonics(s[0], 20)
            p = self.compare_harmonics_to_spikes(spectrum, spikes_distr, harmonics, len(spikes))
            print(s[0], p)
            note_probability.append([s[0], p])

        print(sum(list(map(lambda x: x[1], note_probability))))
        plot.plot(
            spectrum, amplitude, 'r',
            peaks_f, peaks_a, 'b*',
            spikes_f, spikes_n, 'b',
            spikes_f, spikes_a, 'g^',
            spectrum, spikes_distr, 'g'
        )

        plot.show()
        
        """
        use spearman rank coefficient to determine the likelihood that a peak is the fundamental frequency
        by comparing the rank of the frequencies to that expected of that fundamental frequency.
        """
        return note_probability

    def determine_peaks(self, spectrum, amplitude):
        """
        Code to return the frequencies of the largest peaks.
        The code moves through the amplitudes and determines the gradient from one
        point to the next. If the gradient goes from positive to negative, a maximum has
        been reached. The largest value between two minima is found and the corresponsing frequency
        is added to a list.
        """
        peaks = []
        
        prev_gradient = 0
        prev_min_index = 0
        prev_a = 0

        for i, a in enumerate(amplitude):
            new_gradient = self._gradient(prev_a, a)

            if prev_min_index != i and new_gradient > 0 and prev_gradient <= 0:
                peak = self._find_peak(spectrum, prev_min_index, amplitude[prev_min_index:i])
                
                if peak[0] != None:
                    peaks.append(peak)
                
                prev_min_index = i

            prev_gradient = new_gradient
            prev_a = a

        return peaks

    def _gradient(self, x, y):
        return y - x

    def _find_peak(self, spectrum, initial_index, amplitudes):
        f, a_max, a_sum = None, 0, 0

        for i, a in enumerate(amplitudes):
            if a_max < a:
                a_max = a
                f = spectrum[i + initial_index]
            
            a_sum += a

        a_mean = a_sum / len(amplitudes)

        return f, a_max, a_mean

    def determine_spikes(self, peaks, sample_size, threshold):
        """
        Code to differentiate between what is a spike and what is just a small maximum.
        This is done by finding the ratio of a frequency's amplitude to that of background
        noise close to that frequency.
        """
        spikes = []

        sample_size += 1 if sample_size % 2 == 0 else 0
        lb, ub = 0, sample_size // 2
        noise = sum([p[2] for p in peaks[lb:ub]]) / sample_size

        for i, (f, a, m) in enumerate(peaks):
            noise = self._moving_average(noise, peaks, lb, ub, sample_size)
            
            if a / noise > threshold:
                spikes.append([f, a, noise])
            
            lb = i - (sample_size//2)
            ub = i + (sample_size//2)

            lb = lb if lb > 0 else 0

        return spikes

    def _moving_average(self, noise, peaks, lb, ub, sample_size):
        """
        Method to return the average noise quicker than summing elements in a list.
        Because all values except the lowest value are part of the average, it is quicker
        to simply remove the last value, add the new one and divided by the size of the 
        dataset.
        """
        value_to_remove = peaks[lb - 1][2] if lb > 0 else 0
        value_to_add = peaks[ub][2] if ub < len(peaks) else 0

        new_noise = (sample_size * noise) - value_to_remove + value_to_add

        return new_noise / sample_size

    def compare_harmonics_to_spikes(self, spectrum, distribution, harmonics, spikes_num):

        correlation = 0

        for h in harmonics:
            i = 0
            v = spectrum[i]
            difference = None
            
            while difference == None or abs(h - v) < difference:
                difference = abs(h - v)
                i += 1
                if i !< len(spectrum) - 1:
                    break
                
                v = spectrum[i]
            
            correlation += distribution[i]

        return correlation / spikes_num

    def normal_d_spikes(self, spectrum, spikes, spacing):
        distribution = []
        for x in spectrum:
            p = 0
            for f, a, n in spikes:
                p += e ** -((2 * (x-f) / spacing) ** 2)
            distribution.append(p)

        return distribution

    def harmonics(self, f, n):
        return [f*(h+1) for h in range(n)]