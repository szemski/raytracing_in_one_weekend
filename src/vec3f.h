#pragma once

#include "defs.h"

typedef struct vec3f v3f;
typedef v3f p3f; // point
typedef v3f c3f; // color

struct vec3f
{
    union
    {
        struct
        {
            f32 x;
            f32 y;
            f32 z;
        };
        struct
        {
            f32 r;
            f32 g;
            f32 b;
        };
        struct
        {
            f32 u;
            f32 v;
            f32 w;
        };
        f32 e[3];
    };
};

v3f v3f_add(v3f v, v3f u);
v3f v3f_sub(v3f v, v3f u);
v3f v3f_mul(v3f v, f32 s);
v3f v3f_div(v3f v, f32 s);
v3f v3f_mul_comp(v3f v, v3f u);
v3f v3f_opposite(v3f v);
v3f v3f_cross(v3f v, v3f u);
v3f v3f_unit(v3f v);
v3f v3f_reflect(v3f v, v3f n);

f32 v3f_length(v3f v);
f32 v3f_length_squared(v3f v);
f32 v3f_dot(v3f v, v3f u);

v3f v3f_rand01();
v3f v3f_rand_range(f32 v_min, f32 v_max);
v3f v3f_random_in_unit_sphere();
v3f v3f_random_unit_vector();
v3f v3f_random_on_hemisphere(v3f normal);

bool v3f_near_zero(v3f v);
