#include "stdlib.h"
#include "stdio.h"
#include "defs.h"
#include "vec3f.h"
#include "ray.h"
#include "math.h"


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

bool raytest(hittable_array_list* list, ray* r, f32 t_min, f32 t_max, hit_record* rec)
{
    hit_record tmp_rec;
    bool hit_anything = false;
    f32 closest_t = t_max;

    for (int i = 0; i < (int)list->size; ++i)
    {
        hittable* obj = &list->data[i];
        if (ray_hit(r, t_min, closest_t, obj, &tmp_rec))
        {
            hit_anything = true;
            closest_t = tmp_rec.t;
            *rec = tmp_rec;
        }
    }

    return hit_anything;
}

c3f ray_color(ray* r, hittable_array_list* world)
{
    hit_record rec;
    if (raytest(world, r, 0, INFINITY, &rec))
    {
        const v3f n = rec.normal;
        return v3f_mul((c3f) { .r = n.x + 1.f, .g = n.y + 1.f, .b = n.z + 1.f }, 0.5f);
    }

    const v3f unit_direction = v3f_unit(r->dir);
    const f32 a = 0.5f * (unit_direction.y + 1.f);

    const c3f c1 = v3f_mul((c3f) { .r = 1.f, .g = 1.f, .b = 1.f }, 1.f - a);
    const c3f c2 = v3f_mul((c3f) { .r = 0.5f, .g = 0.7f, .b = 1.f }, a);
    return v3f_add(c1, c2);
}

int main(void)
{
    const f32 aspect_ration = 16.f / 9.f;
    const int image_width = 1200;
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

    hittable_array_list world;
    hittable_array_list_init(&world);

    hittable_array_list_add(&world, (hittable) {
        .type = EHittableType_SPHERE,
        .s = (sphere){ .center = {.x = 0, .y = 0, .z = -1}, .radius = 0.5f }
    });
    hittable_array_list_add(&world, (hittable) {
        .type = EHittableType_SPHERE,
        .s = (sphere){ .center = {.x = 0, .y = -100.5f, .z = -1}, .radius = 100.f }
    });


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
            ray r = { .origin = camera_center, .dir = ray_direction };
            framebuffer[row * image_width + col] = ray_color(&r, &world);
        }
    }
    fprintf_s(stderr, "\rScanline progress... DONE\n");

    save_as_ppm("render.ppm", image_width, image_height, framebuffer);

    free(framebuffer);
    return 0;
}
