
#include "main.h"

void generate_palette(unsigned char palette[256][3]) {
    for (int i = 0; i < 256; i++) {
        num_t t = i / 255.0f;
        palette[i][0] = (uint32_t)(9 * (1 - t) * t * t * t * 255);         // Red
        palette[i][1] = (uint32_t)(15 * (1 - t) * (1 - t) * t * t * 255);  // Green
        palette[i][2] = (uint32_t)(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255); // Blue
    }
}

void draw_mandelbrot(unsigned char* image, num_t left, num_t top, num_t xside, num_t yside) {
    num_t xscale = xside / WIDTH;
    num_t yscale = yside / HEIGHT;
    unsigned char palette[256][3];
    generate_palette(palette);

    #pragma omp parallel for collapse(2) schedule(dynamic, 10)
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            num_t cx = x * xscale + left;
            num_t cy = y * yscale + top;
            num_t zx = 0, zy = 0, temp;
            int i = 0;

            while ((zx * zx + zy * zy < 4.0f) && (i < MAXCOUNT)) {
                temp = zx * zx - zy * zy + cx;
                zy = 2.0f * zx * zy + cy;
                zx = temp;
                i++;
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

int main(int argc, char *argv[]) {
    unsigned char* image = (unsigned char*)malloc(IMAGE_SIZE);

    // num_t left = -2.0f;
    // num_t top = -1.25f;
    // num_t xside = 3.0f;
    // num_t yside = 2.5f;
    // -0.74548 + 0.11669 i @ 0.01276
    // -0.744567 + 0.121201 i @ 0.002

    num_t center_x, center_y , xside;

    if (argc != 4) {
        printf("Wrong number of parameters: got %d, expected: 4\n", argc);
        printf("Usage: ./program_name <center_x> <center_y> <xside>\n");
        exit(1);
    }
    center_x = atof(argv[1]);
    center_y = atof(argv[2]);
    xside    = atof(argv[3]);

    num_t yside = xside * HEIGHT / WIDTH;

    num_t left = center_x - xside / 2;
    num_t top  = center_y - yside / 2;

    draw_mandelbrot(image, left, top, xside, yside);

    char filename[256];
    snprintf(filename, sizeof(filename), "mandelbrot_%+.6f_%+.6f_%g_%s.png", center_x, center_y, xside, type_name);

    // Save the image
    if (stbi_write_png(filename, WIDTH, HEIGHT, CHANNELS, image, WIDTH * CHANNELS)) {
        printf("Image saved to %s\n", filename);
    } else {
        printf("Failed to save image %s\n", filename);
    }

    free(image);
    return 0;
}
