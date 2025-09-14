
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "fractal.hpp"
#include "stb_image_write.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <boost/multiprecision/cpp_dec_float.hpp>

static inline void generate_palette(unsigned char palette[256][3]) {
  for (int i = 0; i < 256; i++) {
    double t = static_cast<double>(i) / 255.0;
    double red = 9.0 * 255.0 * (1.0 - t) * t * t * t;
    double green = 15.0 * 255.0 * (1.0 - t) * (1.0 - t) * t * t;
    double blue = 17.0 * 255.0 * (1.0 - t) * (1.0 - t) * (1.0 - t) * t * 0.5;
    palette[i][0] = static_cast<uint8_t>(red);
    palette[i][1] = static_cast<uint8_t>(green);
    palette[i][2] = static_cast<uint8_t>(blue);
  }
}

template <typename T>
void draw(unsigned char *image, fractal_args_t<T> &fractal_args) {
  T xscale = fractal_args.xside / static_cast<T>(WIDTH);
  T yscale = fractal_args.yside / static_cast<T>(HEIGHT);
  unsigned char palette[256][3];
  generate_palette(palette);

#pragma omp parallel for collapse(2) schedule(dynamic, 50)
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      T cx, cy, zx, zy;
      if (fractal_args.fractal_type == MANDELBROT) {
        cx = x * xscale + fractal_args.left;
        cy = y * yscale + fractal_args.top;
        zx = 0;
        zy = 0;
      } else { // JULIA_SET
        cx = fractal_args.cx;
        cy = fractal_args.cy;
        zx = x * xscale + fractal_args.left;
        zy = y * yscale + fractal_args.top;
      }
      int i;
      for (i = 0; (i < MAXCOUNT) && (zx * zx + zy * zy < static_cast<T>(4));
           i++) {
        T temp = zx * zx - zy * zy + cx;
        zy = static_cast<T>(2) * zx * zy + cy;
        zx = temp;
      }

      int index = 3 * (y * WIDTH + x);
      if (i == MAXCOUNT) {
        image[index + 0] = 0;
        image[index + 1] = 0;
        image[index + 2] = 0;
      } else {
        int color_index = (70 + i) % 255 + 1;
        image[index + 0] = palette[color_index][0];
        image[index + 1] = palette[color_index][1];
        image[index + 2] = palette[color_index][2];
      }
    }
  }
}

void error_message() {
  printf("Unexpected input!\n");
  printf("Usage:\n");
  printf("  ./fractal <dtype> mandelbrot <center_x> <center_y> <xside>\n");
  printf("  ./fractal <dtype> julia_set <center_x> <center_y> <xside> "
         "<complex_x> <complex_y>\n");
  printf("\n");
  printf("Examples:\n");
  printf("  ./fractal cfloat64_11 mandelbrot -0.759 0.000 2.5\n");
  printf("  ./fractal softposit32 julia_set 0 0 0.002 -0.74543 0.11301\n");
  exit(1);
}

template <typename T>
void interpret_args(int argc, char *argv[], fractal_args_t<T> &fractal_args) {

  if (strcmp(argv[2], "mandelbrot") == 0 && argc == 6) {
    fractal_args.fractal_type = MANDELBROT;
  } else if (strcmp(argv[2], "julia_set") == 0 && argc == 8) {
    fractal_args.fractal_type = JULIA_SET;
  } else {
    error_message();
  }
  T center_x = static_cast<T>(atof(argv[3]));
  T center_y = static_cast<T>(atof(argv[4]));

  fractal_args.xside = static_cast<T>(atof(argv[5]));
  fractal_args.yside =
      fractal_args.xside * static_cast<T>(HEIGHT) / static_cast<T>(WIDTH);

  fractal_args.left = center_x - fractal_args.xside / static_cast<T>(2);
  fractal_args.top = center_y - fractal_args.yside / static_cast<T>(2);

  if (fractal_args.fractal_type == JULIA_SET) {
    fractal_args.cx = static_cast<T>(atof(argv[6]));
    fractal_args.cy = static_cast<T>(atof(argv[7]));
  }
}

void write_image(char *argv[], unsigned char *image, fractal_t type) {
  char filename[512];
  if (type == MANDELBROT) {
    snprintf(filename, sizeof(filename), "mandelbrot_%+.6f_%+.6f_%+.6f_%s.png",
             atof(argv[3]), atof(argv[4]), atof(argv[5]), argv[1]);
  } else { // JULIA_SET
    snprintf(filename, sizeof(filename),
             "julia_set_%+.6f_%+.6f_%+.6f_%+.6f_%+.6f_%s.png", atof(argv[3]),
             atof(argv[4]), atof(argv[5]), atof(argv[6]), atof(argv[7]),
             argv[1]);
  }

  // Save the image
  if (stbi_write_png(filename, WIDTH, HEIGHT, CHANNELS, image,
                     WIDTH * CHANNELS)) {
    printf("Image saved to %s\n", filename);
  } else {
    printf("Failed to save image %s\n", filename);
  }
}

template <typename T>
void fractal(int argc, char *argv[], unsigned char *image) {
  fractal_args_t<T> fractal_args;
  interpret_args<T>(argc, argv, fractal_args);
  const char *fr_name = (fractal_args.fractal_type == MANDELBROT) ? "mandelbrot" : "julia_set";
  printf("Running dtype=%s, fractal=%s\n", argv[1], fr_name);
  draw<T>(image, fractal_args);
  write_image(argv, image, fractal_args.fractal_type);
}

int main(int argc, char *argv[]) {
  unsigned char *image = (unsigned char *)malloc(IMAGE_SIZE);
  if (!image) {
    fprintf(stderr, "Failed to allocate image buffer of size %d\n", IMAGE_SIZE);
    return 1;
  }

  if (argc < 2) {
    error_message();
  }

  if (strcmp(argv[1], "posit32_2") == 0) {
    fractal<sw::universal::posit<32, 2>>(argc, argv, image);
  } else if (strcmp(argv[1], "cpp_dec_float_1000") == 0) {
    fractal<cpp_dec_float_1000>(argc, argv, image);
  } else if (strcmp(argv[1], "posit16_1") == 0) {
    fractal<sw::universal::posit<16, 1>>(argc, argv, image);
  } else if (strcmp(argv[1], "posit16_2") == 0) {
    fractal<sw::universal::posit<16, 2>>(argc, argv, image);
  } else if (strcmp(argv[1], "posit16_3") == 0) {
    fractal<sw::universal::posit<16, 3>>(argc, argv, image);
  } else if (strcmp(argv[1], "bfloat16_8") == 0) {
    fractal<sw::universal::bfloat16>(argc, argv, image);
  } else if (strcmp(argv[1], "cfloat64_11") == 0) {
    fractal<double>(argc, argv, image);
  } else if (strcmp(argv[1], "cfloat32_8") == 0) {
    fractal<float>(argc, argv, image);
  } else if (strcmp(argv[1], "cfloat16_5") == 0) {
    fractal<_Float16>(argc, argv, image);
  } else if (strcmp(argv[1], "cfloat36_8") == 0) {
    fractal<sw::universal::cfloat<36, 8, uint32_t>>(argc, argv, image);
  } else if (strcmp(argv[1], "cfloat17_5") == 0) {
    fractal<sw::universal::cfloat<17, 5, uint32_t>>(argc, argv, image);
#ifdef ENABLE_SOFTPOSIT
  } else if (strcmp(argv[1], "softposit32") == 0) {
    fractal<posit32>(argc, argv, image);
  } else if (strcmp(argv[1], "softposit16") == 0) {
    fractal<posit16>(argc, argv, image);
#endif
  } else { // all
    argv[1] = const_cast<char *>("cpp_dec_float_1000");
    fractal<cpp_dec_float_1000>(argc, argv, image);
    argv[1] = const_cast<char *>("posit32_2");
    fractal<sw::universal::posit<32, 2>>(argc, argv, image);
    argv[1] = const_cast<char *>("posit16_1");
    fractal<sw::universal::posit<16, 1>>(argc, argv, image);
    argv[1] = const_cast<char *>("posit16_2");
    fractal<sw::universal::posit<16, 2>>(argc, argv, image);
    argv[1] = const_cast<char *>("posit16_3");
    fractal<sw::universal::posit<16, 3>>(argc, argv, image);
    argv[1] = const_cast<char *>("bfloat16_8");
    fractal<sw::universal::bfloat16>(argc, argv, image);
    argv[1] = const_cast<char *>("cfloat64_11");
    fractal<double>(argc, argv, image);
    argv[1] = const_cast<char *>("cfloat32_8");
    fractal<float>(argc, argv, image);
    argv[1] = const_cast<char *>("cfloat16_5");
    fractal<_Float16>(argc, argv, image);
    argv[1] = const_cast<char *>("cfloat36_8");
    fractal<sw::universal::cfloat<36, 8, uint32_t>>(argc, argv, image);
    argv[1] = const_cast<char *>("cfloat17_5");
    fractal<sw::universal::cfloat<17, 5, uint32_t>>(argc, argv, image);
#ifdef ENABLE_SOFTPOSIT
    argv[1] = const_cast<char *>("softposit32");
    fractal<posit32>(argc, argv, image);
    argv[1] = const_cast<char *>("softposit16");
    fractal<posit16>(argc, argv, image);
#endif
  }

  free(image);
  return 0;
}
