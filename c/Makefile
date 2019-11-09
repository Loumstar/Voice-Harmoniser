CC = gcc
CFLAGS = -Wall -pedantic

PITCH_DETECTION_OBJECT_FILES = \
	Pitch-Detection/frequency_bin_typedef.o \
	Pitch-Detection/fourier_transform.o \
	Pitch-Detection/peaks_correlation.o \
	Pitch-Detection/pitch_detection.o \
	Pitch-Detection/peaks_analyser.o

WAV_OBJECT_FILES = \
	Write-WAV-File/wave_header.o \
	Write-WAV-File/endianness.o \
	Write-WAV-File/wave_file.o \
	Write-WAV-File/wave.o

REHARMONISER_OBJECT_FILE = \
	Arduino-Libraries/reharmoniser.o

EXECUTABLES = testVocalHarmoniser
OBJECT_FILES = vocal_harmoniser.test.o

SUBMODULE_OBJECT_FILES = $(PITCH_DETECTION_OBJECT_FILES) $(WAV_OBJECT_FILES) $(REHARMONISER_OBJECT_FILE)

# Executable
testVocalHarmoniser: vocal_harmoniser.test.o 
	$(CC) -o testVocalHarmoniser vocal_harmoniser.test.o $(SUBMODULE_OBJECT_FILES)

# Object Files
vocal_harmoniser.test.o: vocal_harmoniser.test.c
	$(CC) $(CFLAGS) -o vocal_harmoniser.test.o -c vocal_harmoniser.test.c

.PHONY: clean, all, submodules

clean:
	rm -v $(EXECUTABLES) $(OBJECT_FILES)

all:
	make $(EXECUTABLES)

submodules:
	make --directory=./Arduino-Libraries reharmoniser.o
	make --directory=./Write-WAV-File objects
	make --directory=./Pitch-Detection all