import wave
from numpy import fft as fourier, linspace, absolute, sin, pi, mean

class PitchScaler:
    def __init__(self, audio_filename):
        self.audio_file = wave.open(audio_filename)
        self.frame_rate = self.audio_file.getframerate() #in samples per second
        self.bit_depth = 16 ** self.audio_file.getsampwidth()
        self.audio_length = self.audio_file.getnframes() / self.frame_rate

    def audio_spectrum(self, start, end, clip_length):
        #total number of samples taken for a given
        clip_frames = int(self.frame_rate * clip_length)

        #create x axis, which can calculate the existing frequencies up to half 
        #the sampling frequency, due to Nyquist frequency theorem 
        spectrum = linspace(0, self.frame_rate//2, num=clip_frames//2)
        
        #for the moment, will only create a spectrum for a clip taken {start} 
        #seconds from first byte for {clip_length} number of seconds
        clip_start = start
        
        while clip_start < self.audio_length and clip_start < end:
            self.audio_file.setpos(int(clip_start * self.frame_rate)) #set position to the first frame of the starting second
            clip = list(self.audio_file.readframes(clip_frames)) #read frames in the refresh period starting from the marker
            
            amplitudes = self._decompose(clip, clip_frames)
            print(spectrum, amplitudes)

            clip_start = round(clip_start + clip_length, 2)

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

        return amplitude[:clip_frames//2]

    def determine_pitch(self):
        return None
