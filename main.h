#ifndef __MAIN__H__
#define __MAIN__H__

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
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

#endif // __MAIN__H__
