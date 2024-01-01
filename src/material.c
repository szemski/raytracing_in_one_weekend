#include "material.h"
#include "hittable.h"
#include "ray.h"
#include "vec3f.h"

bool material_scatter(material* mat, ray* r, hit_record* rec, c3f* attenuation, ray* scattered)
{
    switch (mat->type)
    {
    case EMaterialType_LAMBERTIAN:
    {
        v3f scatter_dir = v3f_add(rec->normal, v3f_random_unit_vector());
        if (v3f_near_zero(scatter_dir)) scatter_dir = rec->normal;
        scattered->origin = rec->p;
        scattered->dir = scatter_dir;
        *attenuation = mat->albedo;
        return true;
    }
    case EMaterialType_METAL:
    {
        v3f reflected = v3f_reflect(v3f_unit(r->dir), rec->normal);
        scattered->origin = rec->p;
        scattered->dir = reflected;
        *attenuation = mat->albedo;
        return true;
    }
    default: return false;
    }
}
