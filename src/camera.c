#include "stdio.h"
#include "stdlib.h"
#include "malloc.h"
#include "math.h"
#include "camera.h"
#include "hittable.h"
#include "ray.h"

// Utils
bool raytest(hittable_array_list* list, ray* r, interval t_interval, hit_record* rec);
c3f  ray_color(ray* r, int depth, hittable_array_list* world);
c3f  clamp_color(c3f color);
ray  get_ray(camera* cam, int i, int j);
p3f  pixel_sample_square(camera* cam);
c3f  linear_to_gamma(c3f color);


void camera_initialize(camera* cam)
{
    cam->image_height = max((int)(cam->image_width / cam->aspect_ration), 1);
    cam->center = (p3f){ .x = 0, .y = 0, .z = 0 };

    const f32 focal_length = 1.f;
    const f32 viewport_height = 2.f;
    const f32 viewport_width = viewport_height * ((f32)cam->image_width / cam->image_height);

    const v3f viewport_u = { .x = viewport_width, .y = 0, .z = 0 };
    const v3f viewport_v = { .x = 0, .y = -viewport_height, .z = 0 };

    cam->pixel_delta_u = v3f_div(viewport_u, (f32)cam->image_width);
    cam->pixel_delta_v = v3f_div(viewport_v, (f32)cam->image_height);

    const v3f viewport_upper_left
        = v3f_sub(
            cam->center,
            v3f_add(
                (v3f) { .x = 0, .y = 0, .z = focal_length },
                v3f_mul(
                    v3f_add(viewport_u, viewport_v),
                    0.5f)));
    cam->pixel00_loc = v3f_add(
        viewport_upper_left,
        v3f_mul(
            v3f_add(cam->pixel_delta_u, cam->pixel_delta_v),
            0.5f));

    cam->framebuffer = (c3f*)malloc(cam->image_width * cam->image_height * sizeof(c3f));
    if (!cam->framebuffer) exit(1);
}

void camera_delete(camera* cam)
{
    free(cam->framebuffer);
}

void camera_render(camera* cam, hittable_array_list* world)
{
    for (int row = 0; row < cam->image_height; ++row)
    {
        fprintf_s(stderr, "\rScanline progress... %3d%%", (row * 100) / cam->image_height);
        for (int col = 0; col < cam->image_width; ++col)
        {
            c3f color = { .r = 0, .g = 0, .b = 0 };
            for (int sample = 0; sample < cam->samples_per_px; ++sample)
            {
                ray r = get_ray(cam, col, row);
                color = v3f_add(color, ray_color(&r, cam->max_depth, world));
            }
            cam->framebuffer[row * cam->image_width + col]
                = clamp_color(linear_to_gamma(v3f_div(color, (f32)cam->samples_per_px)));
        }
    }
    fprintf_s(stderr, "\rScanline progress... DONE\n");
}

bool raytest(hittable_array_list* list, ray* r, interval t_interval, hit_record* rec)
{
    hit_record tmp_rec;
    bool hit_anything = false;
    f32 closest_t = t_interval.v_max;

    for (int i = 0; i < (int)list->size; ++i)
    {
        hittable* obj = &list->data[i];
        const interval new_interval = { .v_min = t_interval.v_min, .v_max = closest_t };
        if (ray_hit(r, new_interval, obj, &tmp_rec))
        {
            hit_anything = true;
            closest_t = tmp_rec.t;
            *rec = tmp_rec;
        }
    }

    return hit_anything;
}

c3f ray_color(ray* r, int depth, hittable_array_list* world)
{
    if (depth <= 0) return (c3f) { .r = 0, .g = 0, .b = 0 };

    hit_record rec;
    const interval t_interval = { .v_min = 0.001f, .v_max = INFINITY };
    if (raytest(world, r, t_interval, &rec))
    {
        ray scattered;
        c3f attenuation;
        if (material_scatter(rec.mat, r, &rec, &attenuation, &scattered))
        {
            return v3f_mul_comp(attenuation, ray_color(&scattered, depth - 1, world));
        }
        return (c3f) { .r = 0, .g = 0, .b = 0 };
    }

    const v3f unit_direction = v3f_unit(r->dir);
    const f32 a = 0.5f * (unit_direction.y + 1.f);

    const c3f c1 = v3f_mul((c3f) { .r = 1.f, .g = 1.f, .b = 1.f }, 1.f - a);
    const c3f c2 = v3f_mul((c3f) { .r = 0.5f, .g = 0.7f, .b = 1.f }, a);
    return v3f_add(c1, c2);
}

c3f clamp_color(c3f color)
{
    return (c3f) {
        .r = clamp(color.r, 0.f, 0.999f),
        .g = clamp(color.g, 0.f, 0.999f),
        .b = clamp(color.b, 0.f, 0.999f)
    };
}

ray get_ray(camera* cam, int col, int row)
{
    const v3f pixel_center = v3f_add(
        cam->pixel00_loc,
        v3f_add(
            v3f_mul(cam->pixel_delta_u, (f32)col),
            v3f_mul(cam->pixel_delta_v, (f32)row)));
    const v3f pixel_sample = v3f_add(pixel_center, pixel_sample_square(cam));
    const v3f ray_direction = v3f_sub(pixel_sample, cam->center);
    return (ray) { .origin = cam->center, .dir = ray_direction };
}

p3f pixel_sample_square(camera* cam)
{
    f32 px = -0.5f + rand01();
    f32 py = -0.5f + rand01();

    return v3f_add(
        v3f_mul(cam->pixel_delta_u, px),
        v3f_mul(cam->pixel_delta_v, py));
}

c3f linear_to_gamma(c3f color)
{
    return (c3f) {
        .r = sqrtf(color.r),
        .g = sqrtf(color.g),
        .b = sqrtf(color.b)
    };
}

