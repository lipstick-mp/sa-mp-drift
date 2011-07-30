# This file demonstrates how to compile the DriftPointsCounter project on Linux.
#
# To compile DriftPointsCounter do:
#
# make DriftPointsCounter
#

GPP = g++
GCC = gcc
DriftPointsCounter_OUTFILE = "../Debug/DriftPointsCounter.so"

COMPILE_FLAGS = -m32 -fPIC -c -O3 -w -D LINUX -D PROJECT_NAME=\"DriftPointsCounter\" -I ./amx/

DriftPointsCounter = -D DriftPointsCounter $(COMPILE_FLAGS)

all: DriftPointsCounter

clean:
	-rm -f *~ *.o *.so

DriftPointsCounter: clean
	$(GPP) $(DriftPointsCounter) ./amx/*.c
	$(GPP) $(DriftPointsCounter) ./amx/*.cpp
	$(GPP) $(DriftPointsCounter) *.cpp
	$(GPP) -m32 -O2 -fshort-wchar -shared -o $(DriftPointsCounter_OUTFILE) *.o

