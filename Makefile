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

.PHONY: all compile gen_all_images cfloat64_11 cfloat32_8 cfloat16_5 posit32_2 posit16_1 posit16_2 posit16_3 bfloat16_8 cfloat36_8 cfloat17_5 compare install clean

all: $(OUTDIR) compile gen_all_images compare

$(OUTDIR):
	mkdir -p $(OUTDIR)

compile: $(OUTDIR)
	$(CXX) $(CXXFLAGS) -o $(OUT) $(SRC)

cfloat64_11: $(OUTDIR)
	$(SCRIPT) cfloat64_11

cfloat32_8: $(OUTDIR)
	$(SCRIPT) cfloat32_8

cfloat16_5: $(OUTDIR)
	$(SCRIPT) cfloat16_5

posit32_2: $(OUTDIR)
	$(SCRIPT) posit32_2

posit16_1: $(OUTDIR)
	$(SCRIPT) posit16_1

posit16_2: $(OUTDIR)
	$(SCRIPT) posit16_2

posit16_3: $(OUTDIR)
	$(SCRIPT) posit16_3

bfloat16_8: $(OUTDIR)
	$(SCRIPT) bfloat16_8

cfloat36_8: $(OUTDIR)
	$(SCRIPT) cfloat36_8

cfloat17_5: $(OUTDIR)
	$(SCRIPT) cfloat17_5

gen_all_images:
	$(SCRIPT) all

compare:
	$(PYTHON) compare.py

install:
	pip install numpy opencv-python pillow scikit-image

clean:
	rm -rf $(OUTDIR)
