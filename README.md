# Fractals‑Posit (with SoftPosit)

Generate and compare Mandelbrot and Julia‑set images across multiple numeric formats (IEEE, Universal library types, SoftPosit, and Boost.Multiprecision).

After running, images are written to `build/`. The Python tool `compare.py` computes similarity metrics vs a high‑precision baseline and saves a CSV to `build/comparison_results.csv`.

---

## Supported numeric formats (dtype)

- IEEE: `cfloat64_11` (double), `cfloat32_8` (\_Float16), `cfloat16_5` (\_Float16)
- Universal: `posit32_2`, `posit16_3`, `posit16_2`, `posit16_1`, `bfloat16_8`, `cfloat36_8`, `cfloat17_5`
- SoftPosit: `softposit32`, `softposit16`
- Boost.Multiprecision: `cpp_dec_float_100` ([cpp_dec_float](https://www.boost.org/doc/libs/1_77_0/libs/multiprecision/doc/html/boost_multiprecision/tut/floats/cpp_dec_float.html))

You select the dtype at runtime as the first CLI argument, e.g. `./fractal cpp_dec_float_100 ...`.

---

## Prerequisites

- C++20 compiler with OpenMP
  - macOS: Homebrew LLVM (`brew install llvm libomp`) is auto‑detected
  - Linux: GCC 10+ (with `-fopenmp`) is recommended
- Python 3.8+
  - `numpy`, `opencv‑python`, `pillow`, `scikit‑image`, `scipy`
- Universal Number Library (headers only)
  - Clone `https://github.com/stillwater-sc/universal`
  - Set `UNIVERSAL_INC` to its include path (see Setup)
- Boost.Multiprecision (headers only)
  - Install Boost (e.g., macOS: `brew install boost`, Ubuntu: `sudo apt-get install libboost-dev`)
  - Optionally set `BOOST_INC` if Boost is not in a default include path
- SoftPosit (headers + C sources)
  - Clone `https://github.com/Posit-Foundation/soft-posit-cpp`
  ``` bash
  # Clone SoftPosit with specific commit
  git clone https://github.com/Posit-Foundation/soft-posit-cpp.git
  cd soft-posit-cpp
  git checkout 0ff15525c20c11de588eac4168df03428d9ec287
  cd ..
  ```
  - This repo expects the soft‑posit C/C++ headers and sources from `soft-posit-cpp` to be available
  - Set `SOFTPOSIT_ROOT` to the repo root (must contain `include/` and `src/`)

---

## Setup (reproducible)

1. Clone repos

```bash
git clone https://github.com/Joao-Pedro-Cabral/Fractals-Posit.git
cd Fractals-Posit

# Also have these locally (adjust paths as needed):
# universal: https://github.com/stillwater-sc/universal
# soft-posit-cpp: https://github.com/Posit-Foundation/soft-posit-cpp
```

2. Install Python packages

```bash
make install
```

3. Set include/library paths (one‑time per shell)

```bash
# Path to universal headers (the directory that contains sw/universal/...)
export UNIVERSAL_INC="/absolute/path/to/universal/include/sw"

# Path to soft-posit-cpp root (must have include/ and src/)
export SOFTPOSIT_ROOT="/absolute/path/to/soft-posit-cpp"

# Optional: path to Boost headers if not in a default include path
export BOOST_INC="/opt/homebrew/include"
```

Notes (macOS):

- Ensure Homebrew LLVM and libomp are installed: `brew install llvm libomp`
- The Makefile will auto‑detect Homebrew LLVM and link against libomp
- You can override compilers via `CXX`/`CC` if needed

---

## Build and run

Build the fractal generator and the SoftPosit static library, then run a few images:

```bash
make UNIVERSAL_INC="$UNIVERSAL_INC" SOFTPOSIT_ROOT="$SOFTPOSIT_ROOT" compile

# Example: generate a small set of images from the build directory
cd build
./fractal cfloat64_11 mandelbrot -0.759 0.000 2.500
./fractal softposit32  mandelbrot -0.759 0.000 2.500
./fractal softposit16  mandelbrot -0.759 0.000 2.500
./fractal posit32_2    mandelbrot -0.759 0.000 2.500
./fractal cfloat32_8   mandelbrot -0.759 0.000 2.500
```

Or use the convenience script to generate multiple parameter sets:

```bash
# from the repo root
./run.sh all     # builds & runs many parameter sets for each dtype
./run.sh ieee    # builds & runs only IEEE float/double/half sets
./run.sh compare # just runs comparison on existing images in build/
```

Make targets:

- `make all` — build, run all dtypes, then compare
- `make cfloat64_11|cfloat32_8|cfloat16_5|posit32_2|posit16_2|posit16_3|posit16_1|bfloat16_8|cfloat36_8|cfloat17_5|softposit32|softposit16|cpp_dec_float_100` — run a single dtype
- `make compare` — compute metrics over images in `build/`
- `make clean` — remove `build/`

---

## Program usage

Binary runs from `build/` and takes the dtype as first argument:

```bash
./fractal <dtype> mandelbrot <center_x> <center_y> <xside>
./fractal <dtype> julia_set  <center_x> <center_y> <xside> <julia_cx> <julia_cy>
```

Examples:

```bash
./fractal cfloat64_11 mandelbrot -0.74548 0.11669 0.01276
./fractal softposit32 julia_set  0.00000  0.00000 3.00000  -0.4 -0.59
```

Output naming convention:

- Mandelbrot: `mandelbrot_<cx>_<cy>_<xside>_<dtype>.png`
- Julia set: `julia_set_<cx>_<cy>_<xside>_<jc_x>_<jc_y>_<dtype>.png`

`cpp_dec_float_100` serves as the comparison baseline in `compare.py`. See Boost docs: [cpp_dec_float](https://www.boost.org/doc/libs/1_77_0/libs/multiprecision/doc/html/boost_multiprecision/tut/floats/cpp_dec_float.html).

---

## Comparison metrics (`compare.py`)

For each parameter group (same location/scale, possibly same Julia constant), the following metrics are computed against the baseline image (`cpp_dec_float_100`):

- `ssim` — Structural Similarity (higher is better)
- `psnr` — Peak Signal‑to‑Noise Ratio in dB (higher is better)
- `mse` — Mean Squared Error (lower is better)
- `hist` — 3D RGB histogram correlation (higher is better)
- `sift` — SIFT keypoint match rate (higher is better)
- `fft_rmse` — RMSE of log‑magnitude FFTs (lower is better)

Results are written to `build/comparison_results.csv` with headers:

```
parameters, cfloat32_8_ssim, cfloat16_5_ssim, ..., softposit16_fft_rmse
```

If a dtype image is missing for a parameter group, the CSV cell is left empty for that metric.

---

## Implementation notes

- Image size: `WIDTH=800`, `HEIGHT=600`, RGB PNG
- Max iterations: `MAXCOUNT=1000`
- Coordinate setup:

```bash
yside = xside * HEIGHT / WIDTH
left  = center_x - xside / 2
top   = center_y - yside / 2

// Mandelbrot
cx = x * (xside / WIDTH)  + left
cy = y * (yside / HEIGHT) + top

// Julia
zx = x * (xside / WIDTH)  + left
zy = y * (yside / HEIGHT) + top
```

---

## Troubleshooting

- OpenMP on macOS: install `llvm` and `libomp` via Homebrew. The Makefile auto‑detects and links to libomp.
- SoftPosit headers: ensure `SOFTPOSIT_ROOT` points to a tree with `include/` and `src/`. The Makefile adds both `include/` and `include/8086‑SSE/` to the include path.
- Universal headers: set `UNIVERSAL_INC` to the directory that contains `sw/universal/...` (typically `<repo>/include/sw`).
- Python: if OpenCV SIFT is unavailable, install `opencv-contrib-python` or keep `opencv-python` (which ships SIFT in recent versions).

---

## References

Fractal viewpoints used in `run.sh` were sourced from:

<https://www.mrob.com/pub/muency/seahorsevalley.html>
<https://en.wikipedia.org/wiki/Julia_set>
<https://paulbourke.net/fractals/juliaset/index.html>
<https://paulbourke.net/fractals/mandelbrot/>
<https://forums.raspberrypi.com/viewtopic.php?t=21029>
<https://github.com/Posit-Foundation/soft-posit-cpp/blob/refactor-cpp-soft-posits/README.md>

---
