import wave
from numpy import fft as fourier, linspace, absolute, sin, pi, mean, e
import matplotlib.pyplot as plot

class PitchScaler:
    def __init__(self, audio_filename):
        self.audio_file = wave.open(audio_filename)
        self.frame_rate = self.audio_file.getframerate() #in samples per second
        self.bit_depth = 16 ** self.audio_file.getsampwidth()
        self.audio_length = self.audio_file.getnframes() / self.frame_rate

        self.mean_sample_size = 11
        self.threshold = 3

        self.spacing = 50

        self.harmonics_arr_size = 20
        self.spikes_arr_size = 20

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
        spikes = self.get_peaks_in_order_of_spike(peaks)
        distribution = self.spike_distribution(spectrum, spikes)
        
        note_probability = []
        
        for s in spikes:
            harmonics = self.harmonics(s[0])
            p = self.compare_harmonics_to_spikes(spectrum, distribution, harmonics, len(spikes))
            print(s[0], p)
            note_probability.append(tuple([s[0], p]))

        peaks_f = list(map(lambda x: x[0], peaks))
        peaks_a = list(map(lambda x: x[1], peaks))

        spikes_f = list(map(lambda x: x[0], spikes))
        spikes_a = list(map(lambda x: x[1], spikes))
        spikes_n = list(map(lambda x: x[2], spikes))

        print(sum(list(map(lambda x: x[1], note_probability))))
        
        plot.plot(
            spectrum, amplitude, 'r',
            peaks_f, peaks_a, 'b*',
            spikes_f, spikes_n, 'b',
            spikes_f, spikes_a, 'g^',
            spectrum, distribution, 'g'
        )

        plot.show()
        
        return note_probability

    def determine_peaks(self, spectrum, amplitude):
        """
        Method that returns the frequencies of the largest peaks.
        
        The rate of change of amplitude between neighbouring frequencies is calculated.
        If the gradient goes from negative to positive, a minimum has been reached. 
        
        The largest value between two minima is found and the corresponsing frequency
        is added to a list.
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

    def _gradient(self, x, y):
        return y - x

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
        Method to differentiate between what is a spike and what is a small maximum.
        
        The ratio of a frequency's amplitude to that of background noise close to that 
        frequency is a good indicator of a spike, and if the ratio is larger than the
        threshold, it is returned in a list of spikes.
        """
        spikes = []
        lb, ub = (0, self.mean_sample_size)
        noise = sum([p[2] for p in peaks[lb:ub]]) / self.mean_sample_size

        for i, (f, a, _) in enumerate(peaks):
            noise = self._moving_average(noise, peaks, lb, ub)
            
            if a / noise > self.threshold:
                spikes.append([f, a, noise])
            
            if i > self.mean_sample_size // 2:
                lb, ub = lb + 1, ub + 1

        return spikes

    def get_peaks_in_order_of_spike(self, peaks):
        """
        Method that returns the peaks in order of the ratio between their amplitude
        and that of frequencies local to it, which is used to indicate whether it is
        a spike.

        Return an array of tuples up to a maximum number to reduce computing workload
        later.
        """
        spikes = []
        #as an equal number of frequencies should included in the mean calculation either
        #side of the peak, number is made odd to include peak frequency.
        lb, ub = 0, self.mean_sample_size
        noise = sum([p[2] for p in peaks[lb:ub]]) / self.mean_sample_size

        for i, (f, a, _) in enumerate(peaks):
            #as consecutive averages contain some of the same data points, as opposed to
            #manipulating the same values, only the values to be removed/added are manipulated.
            noise = self._moving_average(noise, peaks, lb, ub)
            #all peaks are added and then ranked in terms of their 'spikeiness'
            spikes.append([f, a, noise])
            #bounds for the calculated mean move with each peak.
            if i > self.mean_sample_size // 2:
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

        new_noise = (self.mean_sample_size * noise) - old_value + new_value

        return new_noise / self.mean_sample_size

    def compare_harmonics_to_spikes(self, spectrum, distribution, harmonics, spikes_num):
        #correlation is a value that determines how close to the actual peaks a certain 
        #frequency's harmonics are.
        correlation = 0
        for h in harmonics:
            i, f = 0, spectrum[0]
            difference = None
            while difference == None or abs(h - f) < difference:
                difference = abs(h - f)
                i += 1
                if i == len(spectrum) - 1:
                    break
                f = spectrum[i]
            
            correlation += distribution[i]

        return correlation / spikes_num

    def spike_distribution(self, spectrum, spikes):
        distribution = []
        
        for x in spectrum:
            p = 0
            for s in spikes:
                p += e ** -((2 * (x-s[0]) / self.spacing) ** 2)
            
            distribution.append(p)

        return distribution

    def harmonics(self, f):
        return [f*(h+1) for h in range(self.harmonics_arr_size)]