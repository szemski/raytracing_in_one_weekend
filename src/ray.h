#pragma once

#include "defs.h"
#include "vec3f.h"
#include "hittable.h"

typedef struct ray ray;
typedef struct interval interval;

struct ray
{
    p3f origin;
    v3f dir;
};

struct interval
{
    f32 v_min;
    f32 v_max;
};


extern const interval interval_universe;
extern const interval interval_empty;

v3f  ray_at(ray* r, f32 t);
bool ray_hit(ray* r, interval t_interval, hittable* obj, hit_record* rec);

inline bool interval_contains(interval i, f32 v) { return i.v_min <= v && v <= i.v_max; }
inline bool interval_surrounds(interval i, f32 v) { return i.v_min < v && v < i.v_max; }
