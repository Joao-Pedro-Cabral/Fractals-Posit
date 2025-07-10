#ifndef __MAIN__H__
#define __MAIN__H__

#include <universal/number/posit/posit.hpp>

#define WIDTH 800
#define HEIGHT 600
#define CHANNELS 3
#define IMAGE_SIZE (WIDTH*HEIGHT*CHANNELS)
#define MAXCOUNT 1000

#ifdef __USE_POSIT_32__ 
typedef sw::universal::posit<32,2> num_t;
static const char type_name[10] = "posit32_2";
#elif __USE_DOUBLE__
typedef double num_t;
static const char type_name[10] = "double";
#else // __USE_FLOAT__
typedef float num_t;
static const char type_name[10] = "float";
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
