#include "stdlib.h"
#include "stdio.h"
#include "defs.h"
#include "vec3f.h"
#include "camera.h"
#include "hittable.h"
#include "material.h"


void save_as_ppm(
    const char* filename,
    int image_width,
    int image_height,
    c3f* framebuffer);

int main(void)
{
    material material_ground = {
        .type = EMaterialType_LAMBERTIAN,
        .lambertian = {.albedo = {.r = 0.8f, .g = 0.8f, .b = 0.0f}}
    };

    material material_center = {
        .type = EMaterialType_LAMBERTIAN,
        .lambertian = {.albedo = {.r = 0.1f, .g = 0.2f, .b = 0.5f}}
    };
    material material_left = {
        .type = EMaterialType_DIELECTRIC,
        .dielectric = {.ir = 1.5}
    };
    material material_right = {
        .type = EMaterialType_METAL,
        .metal = {
            .albedo = {.r = 0.8f, .g = 0.6f, .b = 0.2f},
            .fuzz = .0f
        }
    };

    hittable_array_list world;
    hittable_array_list_init(&world);

    hittable_array_list_add(&world, (hittable) {
        .type = EHittableType_SPHERE,
        .s = (sphere){
            .center = {.x = 0, .y = 0, .z = -1},
            .radius = 0.5f,
            .mat = material_center
        }
    });
    hittable_array_list_add(&world, (hittable) {
        .type = EHittableType_SPHERE,
        .s = (sphere){
            .center = {.x = -1, .y = 0, .z = -1},
            .radius = 0.5f,
            .mat = material_left
        }
    });
    hittable_array_list_add(&world, (hittable) {
        .type = EHittableType_SPHERE,
        .s = (sphere){
            .center = {.x = -1, .y = 0, .z = -1},
            .radius = -0.4f,
            .mat = material_left
        }
    });
    hittable_array_list_add(&world, (hittable) {
        .type = EHittableType_SPHERE,
        .s = (sphere){
            .center = {.x = 1, .y = 0, .z = -1},
            .radius = 0.5f,
            .mat = material_right
        }
    });
    hittable_array_list_add(&world, (hittable) {
        .type = EHittableType_SPHERE,
        .s = (sphere){
            .center = {.x = 0, .y = -100.5f, .z = -1},
            .radius = 100.f,
            .mat = material_ground
        }
    });


    camera cam = {
        .aspect_ration = 16.f / 9.f,
        .image_width = 800,
        .samples_per_px = 100,
        .max_depth = 50
    };
    camera_initialize(&cam);
    camera_render(&cam, &world);

    save_as_ppm("render.ppm", cam.image_width, cam.image_height, cam.framebuffer);

    camera_delete(&cam);
    return 0;
}

void save_as_ppm(
    const char* filename,
    int image_width,
    int image_height,
    c3f* framebuffer)
{
    fprintf_s(stderr, "\rSaving PPM file... INIT");
    FILE* file = NULL;
    fopen_s(&file, filename, "w");

    if (!file)
    {
        fprintf_s(stderr, "\rSaving PPM file... FAIL\n");
        exit(1);
    }

    fprintf_s(file, "P3\n%d %d\n255\n", image_width, image_height);

    for (int row = 0; row < image_height; ++row)
    {
        fprintf_s(stderr, "\rSaving PPM file... %3d%%", (row * 100) / image_height);
        for (int col = 0; col < image_width; ++col)
        {
            const v3f sc = v3f_mul(framebuffer[row * image_width + col], 255.999f);
            fprintf_s(file, "%d %d %d\n", (int)sc.x, (int)sc.y, (int)sc.z);
        }
    }
    fclose(file);
    fprintf_s(stderr, "\rSaving PPM file... DONE\n");
}

