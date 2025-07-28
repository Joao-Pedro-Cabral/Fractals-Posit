#ifndef __MAIN__H__
#define __MAIN__H__

#include <universal/number/posit/posit.hpp>
#include <universal/number/bfloat/bfloat.hpp>
#include <universal/number/cfloat/cfloat.hpp>

#define WIDTH 800
#define HEIGHT 600
#define CHANNELS 3
#define IMAGE_SIZE (WIDTH*HEIGHT*CHANNELS)
#define MAXCOUNT 1000

#ifdef __USE_POSIT_32_2__ 
typedef sw::universal::posit<32,2> num_t;
static const char type_name[20] = "posit32_2";
#elif __USE_POSIT_16_2__ 
typedef sw::universal::posit<16,2> num_t;
static const char type_name[20] = "posit16_2";
#elif __USE_BFLOAT_16__
typedef sw::universal::bfloat16 num_t;
static const char type_name[20] = "bfloat16";
#elif __USE_DOUBLE__
typedef double num_t;
static const char type_name[20] = "double";
#elif __USE_HALF__
typedef _Float16 num_t;
static const char type_name[20] = "half";
#elif __USE_FLOAT__
typedef float num_t;
static const char type_name[20] = "float";
#elif __USE_CFLOAT_36_8__
typedef sw::universal::cfloat<36, 8, uint32_t, true, false, false> num_t;
static const char type_name[20] = "cfloat36_8";
#elif __USE_CFLOAT_17_5__
typedef sw::universal::cfloat<17, 5, uint32_t, true, false, false> num_t;
static const char type_name[20] = "cfloat17_5";
#endif

typedef enum {
  MANDELBROT,
  JULIA_SET
} fractal_t;

typedef struct {
  num_t left;
  num_t top;
  num_t xside;
  num_t yside;
  num_t cx;
  num_t cy;
  fractal_t fractal_type; 
} fractal_args_t;

#endif // __MAIN__H__
