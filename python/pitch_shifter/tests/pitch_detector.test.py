from pitch_detector import PitchDetector
import unittest, math

def create_signal(freqs, length, rate):
    t, signal = 0, []
    while t < length:
        amplitude = 0
        for f, a in freqs:
            amplitude += a * math.sin(2 * math.pi * f)
        signal.append(amplitude)
        t += rate
    return signal


class TestPitchDetectorMethods(unittest.TestCase):
    def smokeTest(self):
        smoke_test = PitchDetector('./Testing many notes.wav')
        self.assertIsInstance(smoke_test, PitchDetector)

    def invalidWavPath(self):
        invalid_instance = PitchDetector('')
        self.assertRaises(ValueError)

    def basicFourierTransform(self):
        frequencies = [(50, 2), (75, 1)]
        signal = create_signal(frequencies, 0.2, 44100)
        test_transform = PitchDetector('./Testing many notes.wav')
        test_transform.bit_depth = 1
        test_transform.frame_rate = 44100
        test_transform.clip_length = 0.2
        self.assertAlmostEqual(frequencies, test_transform._decompose(signal, 0.2 * 44100))

if __name__ == '__main__':
    unittest.main()