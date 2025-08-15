
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "fractal.hpp"
#include "stb_image_write.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

template<typename T>
void generate_palette(unsigned char palette[256][3]) {
    for (int i = 0; i < 256; i++) {
        T t = static_cast<T>(i) / static_cast<T>(255);
        T red = static_cast<T>(9)*static_cast<T>(255);
        T green = static_cast<T>(15)*static_cast<T>(255);
        T blue = static_cast<T>(17)*static_cast<T>(255)/static_cast<T>(2);
        red *= (1 - t) * t * t * t;
        green *= (1 - t) * (1 - t) * t * t;
        blue *= (1 - t) * (1 - t) * (1 - t) * t;
        // Some implementations from Universal doesn't have cast to integer types
        // Then I cast to double and implicitly to uint8_t
        palette[i][0] = static_cast<uint8_t>(static_cast<double>(red));
        palette[i][1] = static_cast<uint8_t>(static_cast<double>(green));
        palette[i][2] = static_cast<uint8_t>(static_cast<double>(blue));
    }
}

template<typename T>
void draw(unsigned char* image, fractal_args_t<T>& fractal_args) {
    T xscale = fractal_args.xside / static_cast<T>(WIDTH);
    T yscale = fractal_args.yside / static_cast<T>(HEIGHT);
    unsigned char palette[256][3];
    generate_palette<T>(palette);

    #pragma omp parallel for collapse(2) schedule(dynamic, 100)
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            T cx, cy, zx, zy;
            if(fractal_args.fractal_type == MANDELBROT) {
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
            for(i = 0; (i < MAXCOUNT) && (zx * zx + zy * zy < static_cast<T>(4)); i++) {
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
    printf("Usage:\n./program_name mandelbrot <center_x> <center_y> <xside>\n");
    printf("./program_name julia_set <center_x> <center_y> <xside> <complex_x> <complex_y>\n");
    exit(1);
}

template<typename T>
void interpret_args(int argc, char *argv[], fractal_args_t<T>& fractal_args) {

    if(strcmp(argv[1], "mandelbrot") == 0 && argc == 5) {
        fractal_args.fractal_type = MANDELBROT;
    } else if(strcmp(argv[1], "julia_set") == 0 && argc == 7) {
        fractal_args.fractal_type = JULIA_SET;
    } else {
        error_message();
    }
    T center_x = static_cast<T>(atof(argv[2]));
    T center_y = static_cast<T>(atof(argv[3]));

    fractal_args.xside = static_cast<T>(atof(argv[4]));
    fractal_args.yside = fractal_args.xside * static_cast<T>(HEIGHT) / static_cast<T>(WIDTH);

    fractal_args.left = center_x - fractal_args.xside / static_cast<T>(2);
    fractal_args.top  = center_y - fractal_args.yside / static_cast<T>(2);

    if(fractal_args.fractal_type == JULIA_SET) {
        fractal_args.cx = static_cast<T>(atof(argv[5]));
        fractal_args.cy = static_cast<T>(atof(argv[6])); 
    }
}

template<typename T>
fractal_t fractal(int argc, char *argv[], unsigned char* image) {
    fractal_args_t<T> fractal_args;
    interpret_args<T>(argc, argv, fractal_args);
    draw<T>(image, fractal_args);
    return fractal_args.fractal_type;
}

int main(int argc, char *argv[]) {
    unsigned char* image = (unsigned char*)malloc(IMAGE_SIZE);

    if (argc == 0) {
        error_message();
    }

    fractal_t type;

    if(strcmp(argv[1], "posit32_2") == 0) {
        type = fractal<sw::universal::posit<32,2>>(argc, argv, image);
    } else if(strcmp(argv[1], "posit16_2") == 0) {
        type = fractal<sw::universal::posit<16,2>>(argc, argv, image);
    } else if(strcmp(argv[1], "bfloat16") == 0) {
        type = fractal<sw::universal::bfloat16>(argc, argv, image);
    } else if(strcmp(argv[1], "double") == 0) {
        type = fractal<double>(argc, argv, image);
    } else if(strcmp(argv[1], "float") == 0) {
        type = fractal<float>(argc, argv, image);
    } else if(strcmp(argv[1], "half") == 0) {
        type = fractal<_Float16>(argc, argv, image);
    } else if(strcmp(argv[1], "cfloat36_8") == 0) {
        type = fractal<sw::universal::cfloat<36, 8, uint32_t>>(argc, argv, image);
    } else if(strcmp(argv[1], "cfloat17_5") == 0) {
        type = fractal<sw::universal::cfloat<17, 5, uint32_t>>(argc, argv, image);
    } else {
        error_message();
    }

    char filename[512];
    if(type == MANDELBROT) {
        snprintf(filename, sizeof(filename), "mandelbrot_%+.6f_%+.6f_%+.6f_%s.png", atof(argv[2]), atof(argv[3]), atof(argv[4]), argv[1]);
    } else { // JULIA_SET
        snprintf(filename, sizeof(filename), "julia_set_%+.6f_%+.6f_%+.6f_%+.6f_%+.6f_%s.png", atof(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]), atof(argv[6]), argv[1]);
    }    

    // Save the image
    if (stbi_write_png(filename, WIDTH, HEIGHT, CHANNELS, image, WIDTH * CHANNELS)) {
        printf("Image saved to %s\n", filename);
    } else {
        printf("Failed to save image %s\n", filename);
    }

    free(image);
    return 0;
}
