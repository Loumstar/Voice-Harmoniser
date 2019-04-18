from pitch_scaler import PitchScaler

test = PitchScaler('./Testing many notes.wav')
print(test.determine_pitch(4.15, 0.5))