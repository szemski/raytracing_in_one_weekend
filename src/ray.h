#pragma once

#include "defs.h"
#include "vec3f.h"
#include "hittable.h"

typedef struct ray ray;

struct ray
{
    p3f origin;
    v3f dir;
};

v3f  ray_at(ray* r, f32 t);
bool ray_hit(ray* r, f32 t_min, f32 t_max, hittable* obj, hit_record* rec);

