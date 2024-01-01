#include "material.h"
#include "hittable.h"
#include "ray.h"
#include "vec3f.h"

// Utils
static f32 reflectance(f32 cosine, f32 ref_idx);


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
        *attenuation = mat->lambertian.albedo;
        return true;
    }
    case EMaterialType_METAL:
    {
        v3f reflected = v3f_reflect(v3f_unit(r->dir), rec->normal);
        scattered->origin = rec->p;
        scattered->dir = v3f_add(reflected, v3f_mul(v3f_random_unit_vector(), mat->metal.fuzz));
        *attenuation = mat->metal.albedo;
        return v3f_dot(scattered->dir, rec->normal) > 0.f;
    }
    case EMaterialType_DIELECTRIC:
    {
        *attenuation = (c3f){ .r = 1.0f, .g = 1.0f, .b = 1.0f };
        f32 refraction_ratio = rec->front_face
            ? (1.0f / mat->dielectric.ir)
            : mat->dielectric.ir;

        v3f unit_direction = v3f_unit(r->dir);
        f32 cos_theta = fminf(v3f_dot(v3f_opposite(unit_direction), rec->normal), 1.0f);
        f32 sin_theta = sqrtf(1.0f - cos_theta * cos_theta);
        bool cannot_refract = refraction_ratio * sin_theta > 1.0f;

        scattered->origin = rec->p;
        scattered->dir = cannot_refract || reflectance(cos_theta, refraction_ratio) > rand01()
            ? v3f_reflect(unit_direction, rec->normal)
            : v3f_refract(unit_direction, rec->normal, refraction_ratio);

        return true;
    }
    default: return false;
    }
}

f32 reflectance(f32 cosine, f32 ref_idx) {
    // Use Schlick's approximation for reflectance.
    f32 r0 = (1.f - ref_idx) / (1.f + ref_idx);
    r0 = r0 * r0;
    return r0 + (1.f - r0) * powf((1.f - cosine), 5.f);
}

