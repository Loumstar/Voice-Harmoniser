from pitch_detector import PitchDetector

test = PitchDetector('../../01 Raise Your Weapon.wav')
test.print = True

print(test.get_pitch(16.50))