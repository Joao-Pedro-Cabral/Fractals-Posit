#ifndef __FRACTAL__H__
#define __FRACTAL__H__

#include <universal/number/bfloat/bfloat.hpp>
#include <universal/number/cfloat/cfloat.hpp>
#include <universal/number/posit/posit.hpp>

#define WIDTH 800
#define HEIGHT 600
#define CHANNELS 3
#define IMAGE_SIZE (WIDTH * HEIGHT * CHANNELS)
#define MAXCOUNT 1000

typedef enum { MANDELBROT, JULIA_SET } fractal_t;

template <typename T> struct fractal_args_t {
  T left;
  T top;
  T xside;
  T yside;
  T cx;
  T cy;
  fractal_t fractal_type;
};

#endif // __FRACTAL__H__
