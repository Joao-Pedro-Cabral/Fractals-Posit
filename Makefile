# Compiler and flags
CXX := g++
CXXFLAGS := -O2 -std=c++20 -fopenmp

# Sources and output
SRC := main.cpp
OUT := mandelbrot

# Script
SCRIPT := ./run.sh

# Python
PYTHON := python3.11

.PHONY: all run

all: float double posit_32 compare

float:
	$(CXX) $(CXXFLAGS) -D__USE_FLOAT__ -o mandelbrot main.cpp
	$(SCRIPT)

double:
	$(CXX) $(CXXFLAGS) -D__USE_DOUBLE__ -o mandelbrot main.cpp
	$(SCRIPT)

posit_32:
	$(CXX) $(CXXFLAGS) -D__USE_POSIT_32__ -o mandelbrot main.cpp
	$(SCRIPT)

compare:
	$(PYTHON) compare.py

clean:
	rm -f $(OUT) *.png *.csv
