#pragma once

#include "stddef.h"
#include "defs.h"
#include "vec3f.h"

typedef struct hit_record hit_record;
struct hit_record
{
    p3f p;
    v3f normal;
    f32 t;
    bool front_face;
};

typedef struct sphere sphere;
struct sphere
{
    p3f center;
    f32 radius;
};

typedef enum EHittableType EHittableType;
enum EHittableType
{
    EHittableType_SPHERE
};

typedef struct hittable hittable;
struct hittable
{
    EHittableType type;

    union
    {
        sphere s;
    };
};

typedef struct hittable_array_list hittable_array_list;
struct hittable_array_list
{
    size_t size;
    size_t capacity;
    hittable* data;
};

void hittable_array_list_init(hittable_array_list* list);
void hittable_array_list_delete(hittable_array_list* list);
void hittable_array_list_add(hittable_array_list* list, hittable item);
