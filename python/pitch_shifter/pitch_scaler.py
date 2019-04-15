import wave
from numpy import fft as fourier, linspace, abs
import matplotlib.pyplot as plot

class PitchScaler:
    def __init__(self, audio_filename, note, harmonics):
        self.audio_file = wave.open(audio_filename, 'r')
        self.note = note
        self.harmonics = harmonics

    def note_harmonics(self):
        return [(h+1) * self.note for h in range(self.harmonics)]

    def audio_spectrum(self, start, refresh_period):
        frame_rate = self.audio_file.getframerate()
        sample_frames = int(frame_rate * refresh_period)
        
        #for the moment, will only create a spectrum for the first refresh_period
        self.audio_file.setpos(start * frame_rate)
        
        sample = list(self.audio_file.readframes(sample_frames))

        fft = fourier.fft(
            list(map(lambda x: x / (256 ** self.audio_file.getsampwidth()), sample)
        ))
        
        amplitude = list(
            map(lambda x: 2 * abs(x) / sample_frames, fft)
        )
        spectrum = linspace(0, frame_rate/2, sample_frames/2)

        plot.plot(spectrum, amplitude[:sample_frames//2])
        plot.show()
        
        return None