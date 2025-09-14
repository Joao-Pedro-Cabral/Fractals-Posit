#ifndef __FRACTAL__H__
#define __FRACTAL__H__

#include <universal/number/bfloat16/bfloat16.hpp>
#include <universal/number/cfloat/cfloat.hpp>
#include <universal/number/posit/posit.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#ifdef ENABLE_SOFTPOSIT
#include <softposit_cpp.h>
#endif

namespace bmp = boost::multiprecision;
typedef bmp::number<bmp::cpp_dec_float<1000>> cpp_dec_float_1000;

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
