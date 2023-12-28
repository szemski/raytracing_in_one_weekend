#include "stdio.h"

typedef float  f32;
typedef double f64;

void save_as_ppm(
    const char* filename,
    int image_width,
    int image_height)
{
    fprintf_s(stderr, "\rSaving PPM file... INIT");
    FILE* file = NULL;
    fopen_s(&file, filename, "w");

    if (!file)
    {
        fprintf_s(stderr, "\rSaving PPM file... FAIL\n");
        exit(1);
    }

    fprintf_s(file, "P3\n%d %d\n255\n", image_width, image_height);

    for (int row = 0; row < image_height; ++row)
    {
        fprintf_s(stderr, "\rSaving PPM file... %3d%%", (row * 100) / image_height);
        for (int col = 0; col < image_width; ++col)
        {
            const f32 r = (f32)(col) / (f32)(image_width  - 1);
            const f32 g = (f32)(row) / (f32)(image_height - 1);
            const f32 b = 0.f;

            const f32 factor = 255.999f;
            const int ir = factor * r;
            const int ig = factor * g;
            const int ib = factor * b;

            fprintf_s(file, "%d %d %d\n", ir, ig, ib);
        }
    }
    fclose(file);
    fprintf_s(stderr, "\rSaving PPM file... DONE\n");
}

int main(void)
{
    save_as_ppm("render.ppm", 256, 256);

    return 0;
}
