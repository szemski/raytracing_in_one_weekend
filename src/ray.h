#pragma once

#include "defs.h"
#include "vec3f.h"

typedef struct ray ray;

struct ray
{
    p3f origin;
    v3f dir;
};

inline v3f ray_at(ray* r, f32 t)
{
    return v3f_add(r->origin, v3f_mul(r->dir, t));
}
