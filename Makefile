# Compiler and flags
CXX := g++
CXXFLAGS := -O2 -std=c++20 -fopenmp -Wall -Wextra

# Sources and output
SRC := main.cpp
OUT := fractal

# Script
SCRIPT := ./run.sh

# Python
PYTHON := python3.11

.PHONY: all run

all: float double half posit_32 compare

float:
	$(CXX) $(CXXFLAGS) -D__USE_FLOAT__ -o $(OUT) $(SRC)
	$(SCRIPT)

double:
	$(CXX) $(CXXFLAGS) -D__USE_DOUBLE__ -o $(OUT) $(SRC)
	$(SCRIPT)

half:
	$(CXX) $(CXXFLAGS) -D__USE_HALF__ -o $(OUT) $(SRC)
	$(SCRIPT)

posit_32:
	$(CXX) $(CXXFLAGS) -D__USE_POSIT_32__ -o $(OUT) $(SRC)
	$(SCRIPT)

compare:
	$(PYTHON) compare.py

clean:
	rm -f $(OUT) *.png *.csv
