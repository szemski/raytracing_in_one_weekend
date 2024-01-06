#pragma once

#include "defs.h"
#include "vec3f.h"


typedef struct camera camera;

struct camera
{
    f32 fov;
    p3f lookfrom;
    p3f lookat;
    v3f vup;
    v3f u;
    v3f v;
    v3f w;
    f32 aspect_ration;
    int image_width;
    int image_height;
    v3f center;
    v3f pixel_delta_u;
    v3f pixel_delta_v;
    v3f pixel00_loc;
    int samples_per_px;
    int max_depth;
    bool mt_render;
    int th_count;
    c3f* framebuffer;
};

void camera_initialize(camera* cam);
void camera_delete(camera* cam);
void camera_render(camera* cam, struct hittable_array_list* world);

