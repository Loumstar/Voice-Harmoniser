import wave
from numpy import fft as fourier, linspace, absolute, sin, pi, mean
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
        
        spikes = self.determine_spikes(peaks, 10, 2.5)

        peaks_f = list(map(lambda x: x[0], peaks))
        peaks_a = list(map(lambda x: x[1], peaks))

        spikes_f = list(map(lambda x: x[0], spikes))
        spikes_a = list(map(lambda x: x[1], spikes))
        spikes_n = list(map(lambda x: x[2], spikes))

        plot.plot(
            spectrum, amplitude, 'r',
            peaks_f, peaks_a, 'b*',
            spikes_f, spikes_n, 'b',
            spikes_f, spikes_a, 'g^'
        )

        #plot.show()
        
        """
        use spearman rank coefficient to determine the likelihood that a peak is the fundamental frequency
        by comparing the rank of the frequencies to that expected of that fundamental frequency.
        """
        return spikes

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
        lb = 0
        ub = sample_size // 2
        for i, (f, a, m) in enumerate(peaks):
            noise = sum([p[2] for p in peaks[lb:ub]]) / (sample_size+1)
            
            if a / noise > threshold:
                spikes.append([f, a, noise])
            
            lb = i - (sample_size//2)
            ub = i + (sample_size//2)

            lb = lb if lb > 0 else 0

        return spikes