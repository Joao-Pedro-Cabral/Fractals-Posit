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

.PHONY: all compile float double half posit32_2 posit16_2 bfloat16 cfloat36_8 cfloat17_5 compare install clean

all: $(OUTDIR) compile float double half posit32_2 posit16_2 bfloat16 cfloat36_8 cfloat17_5 compare

$(OUTDIR):
	mkdir -p $(OUTDIR)

compile:
	$(CXX) $(CXXFLAGS) -o $(OUT) $(SRC)

float:
	$(SCRIPT) float

double:
	$(SCRIPT) double

half:
	$(SCRIPT) half

posit32_2:
	$(SCRIPT) posit32_2

posit16_2:
	$(SCRIPT) posit16_2

bfloat16:
	$(SCRIPT) bfloat16

cfloat36_8:
	$(SCRIPT) cfloat36_8

cfloat17_5:
	$(SCRIPT) cfloat17_5

compare:
	$(PYTHON) compare.py

install:
	pip install numpy opencv-python pillow scikit-image

clean:
	rm -rf $(OUTDIR)
