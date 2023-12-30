#pragma once

#include "stdbool.h"
#include "math.h"

typedef float  f32;
typedef double f64;

#define PI 3.1415926535897932385

inline f32 degrees_to_radians(f32 degrees)
{
    return (degrees * (float)PI) / 180.0f;
}
