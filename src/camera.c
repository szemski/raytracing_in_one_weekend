#include "stdio.h"
#include "stdlib.h"
#include "malloc.h"
#include "math.h"
#include "camera.h"
#include "hittable.h"
#include "ray.h"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "process.h"
#include "synchapi.h"


// Utils
#define MAX_THREADS_COUNT 32

typedef struct camera_render_lines_args
{
    camera* cam;
    hittable_array_list* world;
    int start_line;
    int end_line; // excluded

    // statistics
    int current_line;
} camera_render_lines_args;

static bool raytest(hittable_array_list* list, ray* r, interval t_interval, hit_record* rec);
static c3f  ray_color(ray* r, int depth, hittable_array_list* world);
static c3f  clamp_color(c3f color);
static ray  get_ray(camera* cam, int i, int j);
static p3f  pixel_sample_square(camera* cam);
static c3f  linear_to_gamma(c3f color);
static void camera_render_lines(void* args);


void camera_initialize(camera* cam)
{
    cam->image_height = max((int)(cam->image_width / cam->aspect_ration), 1);
    cam->center = cam->lookfrom;

    const f32 focal_length = v3f_length(v3f_sub(cam->lookfrom, cam->lookat));
    const f32 theta = degrees_to_radians(cam->fov);
    const f32 h = tanf(theta / 2.f);
    const f32 viewport_height = 2.f * h * focal_length;
    const f32 viewport_width = viewport_height * ((f32)cam->image_width / cam->image_height);

    cam->w = v3f_unit(v3f_sub(cam->lookfrom, cam->lookat));
    cam->u = v3f_unit(v3f_cross(cam->vup, cam->w));
    cam->v = v3f_cross(cam->w, cam->u);


    const v3f viewport_u = v3f_mul(cam->u, viewport_width);
    const v3f viewport_v = v3f_mul(v3f_opposite(cam->v), viewport_height);

    cam->pixel_delta_u = v3f_div(viewport_u, (f32)cam->image_width);
    cam->pixel_delta_v = v3f_div(viewport_v, (f32)cam->image_height);

    const v3f viewport_upper_left
        = v3f_sub(
            cam->center,
            v3f_add(
                v3f_mul(cam->w, focal_length),
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

    if (cam->mt_render)
    {
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        cam->th_count = info.dwNumberOfProcessors < MAX_THREADS_COUNT
            ? info.dwNumberOfProcessors
            : MAX_THREADS_COUNT;
    }
}

void camera_delete(camera* cam)
{
    free(cam->framebuffer);
}

void camera_render(camera* cam, hittable_array_list* world)
{
    if (cam->mt_render)
    {
        int lines_count = cam->image_height / cam->th_count;
        static camera_render_lines_args args[MAX_THREADS_COUNT];
        static HANDLE th_handles[MAX_THREADS_COUNT];
        for (int t = 0; t < cam->th_count; ++t)
        {
            args[t] = (camera_render_lines_args){
                .cam = cam,
                .world = world,
                .start_line = t * lines_count,
                .end_line = (t + 1 == cam->th_count)
                    ? cam->image_height
                    : (t + 1) * lines_count,
                .current_line = t * lines_count
            };

            th_handles[t] = (HANDLE)_beginthread(camera_render_lines, 0, &args[t]);
        }

        int running_th_count = cam->th_count;
        while (running_th_count > 0)
        {
            int lines_scanned = 0;
            for (int t = 0; t < cam->th_count; ++t)
            {
                lines_scanned += (args[t].current_line - args[t].start_line);
                if (th_handles[t] != NULL && WaitForSingleObject(th_handles[t], 0) != WAIT_TIMEOUT)
                {
                    --running_th_count;
                    th_handles[t] = 0;
                }
            }
            fprintf_s(stderr, "\rScanline progress... %3d%%", (lines_scanned * 100) / cam->image_height);
            Sleep(100);
        }
    }
    else
    {
        camera_render_lines_args args = {
            .cam = cam,
            .world = world,
            .start_line = 0,
            .end_line = cam->image_height
        };

        camera_render_lines(&args);
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

void camera_render_lines(void* args)
{
    struct camera_render_lines_args* rparams = args;
    for (int row = rparams->start_line; row < rparams->end_line; ++row)
    {
        rparams->current_line = row;
        //fprintf_s(stderr, "\rScanline progress... %3d%%", (row * 100) / rparams->cam->image_height);
        for (int col = 0; col < rparams->cam->image_width; ++col)
        {
            c3f color = { .r = 0, .g = 0, .b = 0 };
            for (int sample = 0; sample < rparams->cam->samples_per_px; ++sample)
            {
                ray r = get_ray(rparams->cam, col, row);
                color = v3f_add(color, ray_color(&r, rparams->cam->max_depth, rparams->world));
            }
            rparams->cam->framebuffer[row * rparams->cam->image_width + col]
                = clamp_color(linear_to_gamma(v3f_div(color, (f32)rparams->cam->samples_per_px)));
        }
    }
    //fprintf_s(stderr, "\rScanline progress... DONE\n");
}

#undef WIN32_LEAN_AND_MEAN
#undef MAX_THREADS_COUNT
