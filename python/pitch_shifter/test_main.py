from pitch_scaler import PitchScaler

test = PitchScaler('./Chord progression.wav')
print(test.determine_pitch(20, 0.1))