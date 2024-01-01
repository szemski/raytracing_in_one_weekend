#include "math.h"
#include "vec3f.h"

v3f v3f_add(v3f v, v3f u)
{
    return (v3f) {
        .x = v.x + u.x,
        .y = v.y + u.y,
        .z = v.z + u.z
    };
}

v3f v3f_sub(v3f v, v3f u)
{
    return (v3f) {
        .x = v.x - u.x,
        .y = v.y - u.y,
        .z = v.z - u.z
    };
}

v3f v3f_mul(v3f v, f32 s)
{
    return (v3f) {
        .x = v.x * s,
        .y = v.y * s,
        .z = v.z * s
    };
}

v3f v3f_div(v3f v, f32 s)
{
    return (v3f) {
        .x = v.x / s,
        .y = v.y / s,
        .z = v.z / s
    };
}

v3f v3f_mul_comp(v3f v, v3f u)
{
    return (v3f) {
        .x = v.x * u.x,
        .y = v.y * u.y,
        .z = v.z * u.z
    };
}

v3f v3f_opposite(v3f v)
{
    return (v3f) {
        .x = -v.x,
        .y = -v.y,
        .z = -v.z
    };
}

v3f v3f_cross(v3f v, v3f u)
{
    return (v3f) {
        .x = v.y * u.z - v.z * u.y,
        .y = v.z * u.x - v.x * u.z,
        .z = v.x * u.y - v.y * u.x
    };
}

v3f v3f_unit(v3f v)
{
    return v3f_div(v, v3f_length(v));
}

v3f v3f_reflect(v3f v, v3f n)
{
    return v3f_sub(v, v3f_mul(n, 2.f * v3f_dot(v, n)));
}

v3f v3f_refract(v3f uv, v3f n, f32 etai_over_etat)
{
    f32 cos_theta = fminf(v3f_dot(v3f_opposite(uv), n), 1.f);
    v3f r_out_perp = v3f_mul(v3f_add(uv, v3f_mul(n, cos_theta)), etai_over_etat);
    v3f r_out_para = v3f_mul(n, -sqrtf(fabsf(1.0f - v3f_length_squared(r_out_perp))));
    return v3f_add(r_out_para, r_out_perp);
}

f32 v3f_length(v3f v)
{
    return sqrtf(v3f_length_squared(v));
}

f32 v3f_length_squared(v3f v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

f32 v3f_dot(v3f v, v3f u)
{
    return v.x * u.x + v.y * u.y + v.z * u.z;
}

v3f v3f_rand01()
{
    return (v3f) {
        .x = rand01(),
        .y = rand01(),
        .z = rand01()
    };
}

v3f v3f_rand_range(f32 v_min, f32 v_max)
{
    return (v3f) {
        .x = rand_range(v_min, v_max),
        .y = rand_range(v_min, v_max),
        .z = rand_range(v_min, v_max)
    };
}

v3f v3f_random_in_unit_sphere()
{
    while (true)
    {
        v3f v = v3f_rand_range(-1.f, 1.f);
        if (v3f_length_squared(v) < 1.f) return v;
    }
}

v3f v3f_random_unit_vector()
{
    return v3f_unit(v3f_random_in_unit_sphere());
}

v3f v3f_random_on_hemisphere(v3f normal)
{
    v3f v = v3f_random_unit_vector();
    if (v3f_dot(v, normal) > 0.f) return v;
    return v3f_opposite(v);
}

bool v3f_near_zero(v3f v)
{
    const f32 s = 1e-8f;
    return fabs(v.x) < s && fabs(v.y) < s && fabs(v.z) < s;
}
