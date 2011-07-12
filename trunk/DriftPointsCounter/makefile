# This file demonstrates how to compile the DriftPointsCounter project on Linux.
#
# To compile DriftPointsCounter do:
#
# make DriftPointsCounter
#

GPP = g++
GCC = gcc
DriftPointsCounter_OUTFILE = "DriftPointsCounter.so"

COMPILE_FLAGS = -m32 -fPIC -c -O3 -w -D LINUX -D PROJECT_NAME=\"DriftPointsCounter\" -I ./SDK/amx/

DriftPointsCounter = -D DriftPointsCounter $(COMPILE_FLAGS)

all: DriftPointsCounter

clean:
	-rm -f *~ *.o *.so

DriftPointsCounter: clean
	$(GPP) $(DriftPointsCounter) ./SDK/amx/*.c
	$(GPP) $(DriftPointsCounter) ./SDK/*.cpp
	$(GPP) $(DriftPointsCounter) *.cpp
	$(GPP) -m32 -O2 -fshort-wchar -shared -o $(DriftPointsCounter_OUTFILE) *.o
