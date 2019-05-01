from pitch_detector import PitchDetector
import argparse

parser = argparse.ArgumentParser()

parser.add_argument('audiofile path', type=str, help='The relative or absolute path to a .wav file.')
parser.add_argument('start', type=float, help='The time in seconds to begin analysis.')
parser.add_argument('-p', default=False, const=True, nargs='?', help='Flag to save the analysis as a .pdf file.')

args = vars(parser.parse_args())
test = PitchDetector(args['audiofile path'])
if args['p']:
    test.print = True

if __name__ == '__main__':
    print(test.get_pitch(args['start']))