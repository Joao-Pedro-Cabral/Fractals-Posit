# Fractals‑Posit

A small C++ & Python toolchain to generate and compare Mandelbrot and Julia‑set fractal images using different numeric formats:

- `float`, `_Float16` (half), `double`
- `Posit<32,2>` and `Posit<16,2>`
- `BFloat16`

After running the code, the generated images are written to `build/`. The images can be compared via SSIM, histogram correlation and SIFT using the `compare.py` file, which results are stored in `build/comparison_results.csv`.

---

## Features

- **C++** fractal generator with OpenMP
- Support for IEEE floats, IEEE half‑precision, IEEE doubles, Posit and BFloat16 (via the *universal* library)
- Automatic palette and PNG export (using [*stb\_image\_write*](https://github.com/nothings/stb/tree/master))
- Bash script to run a variety of parameter sets
- Python script to compute image similarity (SSIM, histogram, SIFT)
- For increase performance the data type is defined in compilation time by the defines (`__USE_DATA_TYPE__ `, see `fractal.h`). No overloading

---

## Prerequisites

- A C++20‑capable compiler (e.g. `g++`) with OpenMP support
- [Universal Number Library](https://github.com/stillwater-sc/universal) installed (headers on your include path)
- Native support for IEEE-754 Double, Float and Half
- Python 3.8+ with:
  - `numpy`
  - `opencv‑python`
  - `pillow`
  - `scikit‑image`

---

## Quickstart

1. **Clone** this repo

   ```bash
   git clone https://github.com/Joao-Pedro-Cabral/Fractals-Posit.git
   cd Fractals-Posit
   ```

2. **Install** Python dependencies

   ```bash
   make install
   ```

3. **Build & run** all variants and compare

   ```bash
   make all
   ```

   This will:

   - Compile and run the fractal generator in `build/` for each numeric type
   - Generate PNGs in `build/`
   - Run `compare.py build`, producing `build/comparison_results.csv`

4. **Inspect** your images in `build/` and open the CSV for detailed SSIM/histogram/SIFT scores.

---

## Makefile Targets

- `make all`\
  Compile & run (`float`, `double`, `half`, `posit32_2`, `posit16_2`, `bfloat16`), then compare
- `make float|double|half|posit32_2|posit16_2|bfloat16`\
  Build & run one numeric type
- `make compare`\
  Only compare images stored in `build/`
- `make clean`\
  Remove the entire `build/` directory and all outputs

---

## Fractal Command‑Line

You can also invoke the binary directly:

```bash
g++ -O2 -std=c++20 -fopenmp -D__USE_DATA_TYPE__ -o build/fractal fractal.cpp
build/fractal mandelbrot <center_x> <center_y> <xside>
build/fractal julia_set <center_x> <center_y> <xside> <julia_cx> <julia_cy>
```

E.g.:

```bash
g++ -O2 -std=c++20 -fopenmp -D__USE_POSIT_32_2__ -o build/fractal fractal.cpp
build/fractal mandelbrot -0.74548 0.11669 0.01276
build/fractal julia_set 0.00000 0.00000 3.00000 -0.4 -0.59
```

---

## References

The fractal inputs used in the `run.sh` script are obtained from the following links:

https://www.mrob.com/pub/muency/seahorsevalley.html
https://en.wikipedia.org/wiki/Julia_set
https://paulbourke.net/fractals/juliaset/index.html
https://paulbourke.net/fractals/mandelbrot/
https://forums.raspberrypi.com/viewtopic.php?t=21029

---