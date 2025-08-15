# Compiler and flags
CXX := g++
CXXFLAGS := -O2 -std=c++20 -fopenmp -Wall -Wextra

# Sources and output
SRC := fractal.cpp
OUTDIR := build
OUT := $(OUTDIR)/fractal

# Script
SCRIPT := ./run.sh
PYTHON := python3.11

.PHONY: all

all: $(OUTDIR) float double half posit32_2 posit16_2 bfloat16 cfloat36_8 cfloat17_5 unumI32_4 unumI16_4 unumII16_4 compare

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

bfloat16:
	$(CXX) $(CXXFLAGS) -D__USE_BFLOAT_16__ -o $(OUT) $(SRC)
	$(SCRIPT)

cfloat36_8:
	$(CXX) $(CXXFLAGS) -D__USE_CFLOAT_36_8__ -o $(OUT) $(SRC)
	$(SCRIPT)

cfloat17_5:
	$(CXX) $(CXXFLAGS) -D__USE_CFLOAT_17_5__ -o $(OUT) $(SRC)
	$(SCRIPT)

unumI32_4:
	$(CXX) $(CXXFLAGS) -D__USE_UNUMI_32_4__ -o $(OUT) $(SRC)
	$(SCRIPT)

unumI16_4:
	$(CXX) $(CXXFLAGS) -D__USE_UNUMI_16_4__ -o $(OUT) $(SRC)
	$(SCRIPT)

unumII16_4:
	$(CXX) $(CXXFLAGS) -D__USE_UNUMII_16_4__ -o $(OUT) $(SRC)
	$(SCRIPT)

compare:
	$(PYTHON) compare.py

install:
	pip install numpy opencv-python pillow scikit-image

clean:
	rm -rf $(OUTDIR)
