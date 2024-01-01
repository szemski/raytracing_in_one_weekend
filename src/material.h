#pragma once

#include "defs.h"
#include "vec3f.h"

typedef enum EMaterialType EMaterialType;
typedef struct material material;

enum EMaterialType
{
    EMaterialType_LAMBERTIAN,
    EMaterialType_METAL,
    EMaterialType_DIELECTRIC
};

struct material
{
    EMaterialType type;

    union
    {
        struct lambertian_params
        {
            c3f albedo;
        } lambertian;

        struct metal_params
        {
            c3f albedo;
            f32 fuzz;
        } metal;

        struct dielectric_params
        {
            f32 ir;
        } dielectric;
    };
};


bool material_scatter(
    material* mat,
    struct ray* r,
    struct hit_record* rec,
    c3f* attenuation,
    struct ray* scattered);
