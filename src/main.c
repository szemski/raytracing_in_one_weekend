#include "stdlib.h"
#include "stdio.h"
#include "defs.h"
#include "vec3f.h"
#include "ray.h"

void save_as_ppm(
    const char* filename,
    int image_width,
    int image_height,
    c3f* framebuffer)
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
            const v3f sc = v3f_mul(framebuffer[row * image_width + col], 255.999f);
            fprintf_s(file, "%d %d %d\n", (int)sc.x, (int)sc.y, (int)sc.z);
        }
    }
    fclose(file);
    fprintf_s(stderr, "\rSaving PPM file... DONE\n");
}

c3f ray_color(ray* r)
{
    const v3f unit_direction = v3f_unit(r->dir);
    const f32 a = 0.5f * (unit_direction.y + 1.f);

    const c3f c1 = v3f_mul((c3f) { .r = 1.f, .g = 1.f, .b = 1.f }, 1.f - a);
    const c3f c2 = v3f_mul((c3f) { .r = 0.5f, .g = 0.7f, .b = 1.f }, a);
    return v3f_add(c1, c2);
}

int main(void)
{
    const f32 aspect_ration = 16.f / 9.f;
    const int image_width = 800;
    const int image_height = max((int)(image_width / aspect_ration), 1);

    const f32 focal_point = 1.f;
    const f32 viewport_height = 2.f;
    const f32 viewport_width = viewport_height * ((f32)image_width / image_height);
    const v3f camera_center = { .x = 0, .y = 0, .z = 0 };

    const v3f viewport_u = { .x = viewport_width, .y = 0, .z = 0 };
    const v3f viewport_v = { .x = 0, .y = -viewport_height, .z = 0 };

    const v3f pixel_delta_u = v3f_div(viewport_u, (f32)image_width);
    const v3f pixel_delta_v = v3f_div(viewport_v, (f32)image_height);

    const v3f viewport_upper_left
        = v3f_sub(
            camera_center,
            v3f_add(
                (v3f) { .x = 0, .y = 0, .z = focal_point },
                v3f_mul(
                    v3f_add(viewport_u, viewport_v),
                    0.5f)));
    const v3f pixel00_loc = v3f_add(
        viewport_upper_left,
        v3f_mul(
            v3f_add(pixel_delta_u, pixel_delta_v),
            0.5f));

    c3f* framebuffer = (c3f*)malloc(image_width * image_height * sizeof(c3f));
    if (!framebuffer) exit(1);

    for (int row = 0; row < image_height; ++row)
    {
        fprintf_s(stderr, "\rScanline progress... %3d%%", (row * 100) / image_height);
        for (int col = 0; col < image_width; ++col)
        {
            const v3f pixel_center = v3f_add(
                pixel00_loc,
                v3f_add(
                    v3f_mul(pixel_delta_u, (f32)col),
                    v3f_mul(pixel_delta_v, (f32)row)
                ));
            const v3f ray_direction = v3f_sub(pixel_center, camera_center);
            ray r = { .origin = pixel_center, .dir = ray_direction };
            framebuffer[row * image_width + col] = ray_color(&r);
        }
    }
    fprintf_s(stderr, "\rScanline progress... DONE\n");

    save_as_ppm("render.ppm", image_width, image_height, framebuffer);

    free(framebuffer);
    return 0;
}
