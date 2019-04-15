import pitch_scaler

test = pitch_scaler.PitchScaler('./Chord progression.wav', 440, 20)

print(test.audio_spectrum(30, 0.1))