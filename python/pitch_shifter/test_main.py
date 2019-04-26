from pitch_detector import PitchDetector

test = PitchDetector('./Testing many notes.wav')
test.print = True

print(test.get_pitch(4.50))