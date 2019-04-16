from pitch_scaler import PitchScaler

test = PitchScaler('./python/pitch_shifter/Chord progression.wav')
print(test.audio_spectrum(30, 35, 0.1))