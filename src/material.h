#pragma once

#include "defs.h"
#include "vec3f.h"

typedef enum EMaterialType EMaterialType;
typedef struct material material;

enum EMaterialType
{
    EMaterialType_LAMBERTIAN,
    EMaterialType_METAL
};

struct material
{
    EMaterialType type;

    c3f albedo;
    f32 fuzz;
};


bool material_scatter(
    material* mat,
    struct ray* r,
    struct hit_record* rec,
    c3f* attenuation,
    struct ray* scattered);
