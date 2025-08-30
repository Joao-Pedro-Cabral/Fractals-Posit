# Compiler and platform detection
UNAME_S := $(shell uname -s)

# Default compilers
CXX := g++
CC  := gcc

# OpenMP flags (set per-platform/toolchain)
OPENMP_FLAGS := -fopenmp

ifeq ($(UNAME_S),Darwin)
  # Prefer Homebrew LLVM's clang/clang++ if available
  BREW_PREFIX := $(shell brew --prefix 2>/dev/null)
  LLVM_CLANGXX := $(BREW_PREFIX)/opt/llvm/bin/clang++
  LLVM_CLANGC  := $(BREW_PREFIX)/opt/llvm/bin/clang
  ifneq (,$(wildcard $(LLVM_CLANGXX)))
    CXX := $(LLVM_CLANGXX)
    CC  := $(LLVM_CLANGC)
    # Use libomp from Homebrew
    OPENMP_FLAGS := -fopenmp -I$(BREW_PREFIX)/opt/libomp/include -L$(BREW_PREFIX)/opt/libomp/lib -lomp -Wl,-rpath,$(BREW_PREFIX)/opt/libomp/lib
  else
    # Try Homebrew GCC (adjust version if needed)
    CXX := g++-14
    CC  := gcc-14
    OPENMP_FLAGS := -fopenmp
  endif
endif

# Optional: path to stillwater-sc/universal include directory
# Supply as make UNIVERSAL_INC=/path/to/universal/include
ifdef UNIVERSAL_INC
  UNIVERSAL_INCLUDE := -I$(UNIVERSAL_INC)
endif

# Optional: SoftPosit library root (expects include/ and src/)
# Supply as make SOFTPOSIT_ROOT=/path/to/soft-posit-cpp
ifdef SOFTPOSIT_ROOT
  SOFTPOSIT_INCLUDE := -I$(SOFTPOSIT_ROOT)/include -I$(SOFTPOSIT_ROOT)/include/8086-SSE -DENABLE_SOFTPOSIT
  SOFTPOSIT_CFLAGS  := -O2 -std=c11 -Wall $(SOFTPOSIT_INCLUDE)
  SOFTPOSIT_SRC_DIR := $(SOFTPOSIT_ROOT)/src
  SOFTPOSIT_C_SOURCES := $(wildcard $(SOFTPOSIT_SRC_DIR)/*.c)
  OUTDIR := build
  SOFTPOSIT_OBJS_DIR := $(OUTDIR)/softposit
  SOFTPOSIT_OBJS := $(patsubst $(SOFTPOSIT_SRC_DIR)/%.c,$(SOFTPOSIT_OBJS_DIR)/%.o,$(SOFTPOSIT_C_SOURCES))
  SOFTPOSIT_STATIC := $(OUTDIR)/libsoftposit.a
endif

CXXFLAGS := -O2 -std=c++20 -Wall -Wextra $(OPENMP_FLAGS) $(UNIVERSAL_INCLUDE) $(SOFTPOSIT_INCLUDE)
CPPFLAGS :=

# Sources and output
SRC := fractal.cpp
OUTDIR ?= build
OUT := $(OUTDIR)/fractal

# Script
SCRIPT := ./run.sh
PYTHON := python3

.PHONY: all compile gen_all_images cfloat64_11 cfloat32_8 cfloat16_5 posit32_2 posit16_1 posit16_2 posit16_3 bfloat16_8 cfloat36_8 cfloat17_5 softposit32 softposit16 compare install clean

all: $(OUTDIR) compile gen_all_images compare

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(SOFTPOSIT_OBJS_DIR):
	mkdir -p $(SOFTPOSIT_OBJS_DIR)

compile: $(OUTDIR) $(SOFTPOSIT_STATIC)
	$(CXX) $(CXXFLAGS) -o $(OUT) $(SRC) $(SOFTPOSIT_STATIC)

cfloat64_11: $(OUTDIR) compile
	$(SCRIPT) cfloat64_11

cfloat32_8: $(OUTDIR) compile
	$(SCRIPT) cfloat32_8

cfloat16_5: $(OUTDIR) compile
	$(SCRIPT) cfloat16_5

posit32_2: $(OUTDIR) compile
	$(SCRIPT) posit32_2

posit16_1: $(OUTDIR) compile
	$(SCRIPT) posit16_1

posit16_2: $(OUTDIR) compile
	$(SCRIPT) posit16_2

posit16_3: $(OUTDIR) compile
	$(SCRIPT) posit16_3

bfloat16_8: $(OUTDIR) compile
	$(SCRIPT) bfloat16_8

cfloat36_8: $(OUTDIR) compile
	$(SCRIPT) cfloat36_8

cfloat17_5: $(OUTDIR) compile
	$(SCRIPT) cfloat17_5

softposit32: $(OUTDIR) compile
	$(SCRIPT) softposit32

softposit16: $(OUTDIR) compile
	$(SCRIPT) softposit16

gen_all_images: compile
	$(SCRIPT) all

compare:
	$(PYTHON) compare.py

install:
	pip install numpy opencv-python pillow scikit-image scipy

clean:
	rm -rf $(OUTDIR)

# Build rules for SoftPosit C sources
ifdef SOFTPOSIT_ROOT
$(SOFTPOSIT_OBJS_DIR)/%.o: $(SOFTPOSIT_SRC_DIR)/%.c | $(SOFTPOSIT_OBJS_DIR)
	$(CC) $(SOFTPOSIT_CFLAGS) -c $< -o $@

$(SOFTPOSIT_STATIC): $(SOFTPOSIT_OBJS)
	ar rcs $@ $(SOFTPOSIT_OBJS)
endif
