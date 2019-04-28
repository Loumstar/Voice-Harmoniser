from pitch_detector import PitchDetector
import argparse
"""
parser = argparse.ArgumentParser()

parser.add_argument('audiofile path', help='The relative or absolute path to a .wav file.')
parser.add_argument('start', type=float, help='The time in seconds to begin analysis.')
parser.add_argument('print', '-p', default=False, const=True, help='Flag to save the analysis as a .pdf file.')

args = parser.parse_args()
"""
test = PitchDetector('./Testing many notes.wav')
test.print = True

print(test.get_pitch(4.50))