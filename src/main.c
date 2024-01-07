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
        .lambertian = {.albedo = {.r = 0.5f, .g = 0.5f, .b = 0.5f}}
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
            .center = {.x = 0, .y = -1000.f, .z = 0},
            .radius = 1000.f,
            .mat = material_ground
        }
    });

    for (int a = -11; a < 11; ++a)
    {
        for (int b = -11; b < 11; ++b)
        {
            const f32 choose_mat = rand01();
            const p3f center = {
                .x = a + 0.9f * rand01(),
                .y = 0.2,
                .z = b + 0.9f * rand01()
            };

            if (v3f_length(v3f_sub(center, (p3f) { .x = 4.f, .y = 0.2f, .z = 0.f })) > 0.9f)
            {
                material mat;
                if (choose_mat < 0.8f)
                {
                    mat = (material){
                        .type = EMaterialType_LAMBERTIAN,
                        .lambertian = {
                            .albedo = v3f_mul_comp(
                                (v3f){rand01(), rand01(), rand01()},
                                (v3f){rand01(), rand01(), rand01()})
                        }
                    };
                }
                else if (choose_mat < 0.95f)
                {
                    mat = (material){
                        .type = EMaterialType_METAL,
                        .metal = {
                            .albedo = (v3f){
                                .r = rand_range(0.5f, 1.f),
                                .g = rand_range(0.5f, 1.f),
                                .b = rand_range(0.5f, 1.f)
                            },
                            .fuzz = rand_range(0.f, 0.5f)
                        }
                    };
                }
                else
                {
                    mat = (material){
                        .type = EMaterialType_DIELECTRIC,
                        .dielectric = { .ir = 1.5f }
                    };
                }
                hittable_array_list_add(&world, (hittable) {
                    .type = EHittableType_SPHERE,
                        .s = {
                            .center = center,
                            .radius = 0.2f,
                            .mat = mat
                    }
                });
            }
        }
    }

    hittable_array_list_add(&world, (hittable) {
        .type = EHittableType_SPHERE,
        .s = {
            .center = {.x = 0.f, .y = 1.f, .z = 0.f},
            .radius = 1.f,
            .mat = {
                .type = EMaterialType_DIELECTRIC,
                .dielectric = {.ir = 1.5f }
            }
        }
    });

    hittable_array_list_add(&world, (hittable) {
        .type = EHittableType_SPHERE,
        .s = {
            .center = {.x = -4.f, .y = 1.f, .z = 0.f},
            .radius = 1.f,
            .mat = {
                .type = EMaterialType_LAMBERTIAN,
                .lambertian = {
                    .albedo = {.r = 0.4f, .g =0.2f, .b = 0.1f}
                }
            }
        }
    });

    hittable_array_list_add(&world, (hittable) {
        .type = EHittableType_SPHERE,
        .s = {
            .center = {.x = 4.f, .y = 1.f, .z = 0.f},
            .radius = 1.f,
            .mat = {
                .type = EMaterialType_METAL,
                .metal = {
                    .albedo = {.r = 0.7f, .g =0.6f, .b = 0.5f},
                    .fuzz = 0.f
                }
            }
        }
    });

    camera cam = {
        .fov = 20.f,
        .lookfrom = (p3f){.x = 13.f, .y = 2.f, .z = 3.f},
        .lookat = (p3f){.x = 0, .y = 0, .z = 0.f},
        .vup = (v3f){.x = 0, .y = 1.f, .z = 0},
        .aspect_ration = 16.f / 9.f,
        .image_width = 1200,
        .samples_per_px = 500,
        .defocus_angle = 0.6f,
        .focus_dist = 10.f,
        .max_depth = 50,
        .mt_render = true
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

