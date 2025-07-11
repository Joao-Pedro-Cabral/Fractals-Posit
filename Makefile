# Compiler and flags
CXX := g++
CXXFLAGS := -O2 -std=c++20 -fopenmp -Wall -Wextra

# Sources and output
SRC := fractal.cpp
OUTDIR := build
OUT := $(OUTDIR)/fractal

# Script
SCRIPT := ./run.sh
PYTHON := python3

.PHONY: all clean install float double half posit32_2 posit16_2 compare

all: $(OUTDIR) float double half posit32_2 posit16_2 compare

$(OUTDIR):
	mkdir -p $(OUTDIR)

float:
	$(CXX) $(CXXFLAGS) -D__USE_FLOAT__ -o $(OUT) $(SRC)
	$(SCRIPT)

double:
	$(CXX) $(CXXFLAGS) -D__USE_DOUBLE__ -o $(OUT) $(SRC)
	$(SCRIPT)

half:
	$(CXX) $(CXXFLAGS) -D__USE_HALF__ -o $(OUT) $(SRC)
	$(SCRIPT)

posit32_2:
	$(CXX) $(CXXFLAGS) -D__USE_POSIT_32_2__ -o $(OUT) $(SRC)
	$(SCRIPT)

posit16_2:
	$(CXX) $(CXXFLAGS) -D__USE_POSIT_16_2__ -o $(OUT) $(SRC)
	$(SCRIPT)

compare:
	$(PYTHON) compare.py

install:
	pip install numpy opencv-python pillow scikit-image

clean:
	rm -rf $(OUTDIR)
