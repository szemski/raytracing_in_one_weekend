#pragma once

#include "stdlib.h"
#include "stdbool.h"
#include "math.h"

typedef float  f32;
typedef double f64;

#define PI 3.1415926535897932385

inline f32 degrees_to_radians(f32 degrees)
{
    return (degrees * (float)PI) / 180.0f;
}

inline f32 rand01()
{
    return (f32)rand() / (RAND_MAX + 1.f);
}

inline f32 rand_range(f32 v_min, f32 v_max)
{
    return v_min + (v_max - v_min) * rand01();
}

inline f32 clamp(f32 v, f32 a, f32 b)
{
    return (v < a) ? a : ((v > b) ? b : v);
}
