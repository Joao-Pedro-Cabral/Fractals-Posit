
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "fractal.h"
#include "stb_image_write.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void generate_palette(unsigned char palette[256][3]) {
    for (int i = 0; i < 256; i++) {
        num_t t = i / (num_t)255;
        // Some implementations from Universal doesn't have cast to integer types
        // Then I cast to double and implicitly to uint8_t
        palette[i][0] = (double)(9 * (1 - t) * t * t * t * 255);         // Red
        palette[i][1] = (double)(15 * (1 - t) * (1 - t) * t * t * 255);  // Green
        palette[i][2] = (double)(17 * (1 - t) * (1 - t) * (1 - t) * t * 255/2); // Blue
    }
}

void draw(unsigned char* image, fractal_args_t fractal_args) {
    num_t xscale = fractal_args.xside / WIDTH;
    num_t yscale = fractal_args.yside / HEIGHT;
    unsigned char palette[256][3];
    generate_palette(palette);

    #pragma omp parallel for collapse(2) schedule(dynamic, 100)
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            num_t cx, cy, zx, zy;
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
            // TODO: Universal bfloat16 doesn't work with <
            double z_abs = (double) (zx * zx + zy * zy);
            for(i = 0; (i < MAXCOUNT) && (z_abs < 4); i++) {
                num_t temp = zx * zx - zy * zy + cx;
                zy = 2 * zx * zy + cy;
                zx = temp;
                z_abs = (double) (zx * zx + zy * zy);
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

int main(int argc, char *argv[]) {
    unsigned char* image = (unsigned char*)malloc(IMAGE_SIZE);
    num_t center_x, center_y;

    fractal_args_t fractal_args;
    if (argc == 0) {
        error_message();
    } else if(strcmp(argv[1], "mandelbrot") == 0 && argc == 5) {
        fractal_args.fractal_type = MANDELBROT;
    } else if(strcmp(argv[1], "julia_set") == 0 && argc == 7) {
        fractal_args.fractal_type = JULIA_SET;
    } else {
        error_message();
    }
    center_x = (num_t)atof(argv[2]);
    center_y = (num_t)atof(argv[3]);

    fractal_args.xside = (num_t)atof(argv[4]);
    fractal_args.yside = fractal_args.xside * HEIGHT / WIDTH;

    fractal_args.left = center_x - fractal_args.xside / 2;
    fractal_args.top  = center_y - fractal_args.yside / 2;

    if(fractal_args.fractal_type == JULIA_SET) {
        fractal_args.cx = (num_t)atof(argv[5]);
        fractal_args.cy = (num_t)atof(argv[6]); 
    }

    draw(image, fractal_args);

    char filename[512];
    if(fractal_args.fractal_type == MANDELBROT) {
        snprintf(filename, sizeof(filename), "mandelbrot_%+.6f_%+.6f_%+.6f_%s.png", atof(argv[2]), atof(argv[3]), atof(argv[4]), type_name);
    } else { // JULIA_SET
        snprintf(filename, sizeof(filename), "julia_set_%+.6f_%+.6f_%+.6f_%+.6f_%+.6f_%s.png", atof(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]), atof(argv[6]), type_name);
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
