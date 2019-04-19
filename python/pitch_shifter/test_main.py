from pitch_detector import PitchDetector

print(
    PitchDetector('./Testing many notes.wav')
        .determine_pitch(4.15)
)