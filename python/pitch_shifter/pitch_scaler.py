import wave
from numpy import fft as fourier, linspace, absolute, sin, pi, mean
import matplotlib.pyplot as plot

class PitchScaler:
    def __init__(self, audio_filename, note, harmonics):
        self.audio_file = wave.open(audio_filename)
        self.note = note
        self.harmonics = harmonics

    def note_harmonics(self):
        return [(h+1) * self.note for h in range(self.harmonics)]

    def audio_spectrum(self, start, refresh_period):
        frame_rate = self.audio_file.getframerate() #in samples per second
        sample_frames = int(frame_rate * refresh_period) #total number of samples taken for a given
        bit_depth = (16 ** self.audio_file.getsampwidth())
        
        #for the moment, will only create a spectrum for a clip taken {start} 
        #seconds from first byte for {refresh_period} number of seconds
        self.audio_file.setpos(int(start * frame_rate)) #set position to the first frame of the starting second
        clip = list(self.audio_file.readframes(sample_frames)) #read frames in the refresh period starting from the marker

        spectrum = linspace(0, frame_rate//2, num=sample_frames//2) #create x axis, which can calculate the existing frequencies 
        #up to half the sampling frequency, due to Nyquist frequency theorem 
        
        #apply fast-fourier-transform to determine the amplitude and phase of each frequency, given as a complex number.
        #as x ranges from 0 to 255, the average x will be non zero, giving the whole clip an offset amplitude.
        #the value of x needs to be within 0 and 1, so x is divided by the audio bit depth
        offset = mean(clip) / bit_depth

        fft = fourier.fft(list(map(lambda x: (x / bit_depth) - offset, clip)))

        #phase of each frequency is disregarded by finding the magnitude of the complex number
        #each frequency is divided by the number of sample frames and multiplied by two to account
        #for removing the mirrored frequencies due to Nyquist frequency theory.
        amplitude = list(map(lambda x: 2 * absolute(x) / sample_frames, fft))

        return spectrum, amplitude[:sample_frames//2]