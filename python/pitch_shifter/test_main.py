from pitch_detector import PitchDetector

test = PitchDetector('../../01 Raise Your Weapon.wav')
test.print = True

print(test.determine_pitch(1.00))