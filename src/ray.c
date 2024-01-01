#include "math.h"
#include "ray.h"

// Utils
void set_face_normal(hit_record* rec, ray* r, v3f outward_normal);

const interval interval_universe = { .v_min = -INFINITY, .v_max = INFINITY };
const interval interval_empty = { .v_min = INFINITY, .v_max = -INFINITY };

v3f ray_at(ray* r, f32 t)
{
    return v3f_add(r->origin, v3f_mul(r->dir, t));
}

bool ray_hit(ray* r, interval t_interval, hittable* obj, hit_record* rec)
{
    switch (obj->type)
    {
    case EHittableType_SPHERE:
    {
        sphere* s = &obj->s;
        const v3f oc = v3f_sub(r->origin, s->center);
        const f32 a = v3f_length_squared(r->dir);
        const f32 half_b = v3f_dot(oc, r->dir);
        const f32 c = v3f_length_squared(oc) - s->radius * s->radius;
        const f32 discriminant = half_b * half_b - a * c;

        if (discriminant < 0) return false;

        const f32 sqrtd = sqrtf(discriminant);
        f32 root = (-half_b - sqrtd) / a;
        if (!interval_surrounds(t_interval, root))
        {
            root = (-half_b + sqrtd) / a;
            if (!interval_surrounds(t_interval, root)) return false;
        }

        rec->p = ray_at(r, root);
        rec->t = root;
        const v3f outward_normal = v3f_div(v3f_sub(rec->p, s->center), s->radius);
        set_face_normal(rec, r, outward_normal);
        rec->mat = &s->mat;
        return true;
    }
    default: return false;
    }
}


void set_face_normal(hit_record* rec, ray* r, v3f outward_normal)
{
    rec->front_face = v3f_dot(r->dir, outward_normal) < 0;
    rec->normal = rec->front_face ? outward_normal : v3f_opposite(outward_normal);
}
