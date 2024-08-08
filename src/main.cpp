#include "rtweekend.hpp"

#include "bvh.hpp"
#include "camera.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include "material.hpp"

#include "world_setups.hpp"

#include <iostream>

hittable_list world_setup_rtweekend() {
    hittable_list world;
    auto ground_material = make_shared<lambertian>(color{0.5, 0.5, 0.5});
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int32_t a = -11; a < 11; a++) {
        for (int32_t b = -11; b < 11; b++) {
            float choose_mat = random_float();
            point3 center = point3{static_cast<float>(a), 0.2, static_cast<float>(b)} + 0.9*point3{random_float(), 0, random_float()};

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3{0, random_float(0, 0.5), 0};
                    world.add(make_shared<sphere>(center, center2, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_float(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));
    auto material1_alpha = make_shared<dielectric>(1.0 / 1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 0.8, material1_alpha));
    
    auto material2 = make_shared<lambertian>(color{0.4, 0.2, 0.1});
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color{0.7, 0.6, 0.5}, 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));
    
    world = hittable_list(make_shared<bvh_node>(world));

    return world;
}

int main() {
    hittable_list world = world_setup_rtweekend();

    camera cam;
    cam.aspect_ratio    = 16.0 / 9.0;
    cam.image_width     = 400;
    cam.samples_per_pixel = 50;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = {13, 2, 3};
    cam.lookat   = {0, 0, 0};
    cam.vup      = {0, 1, 0};

    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;

    cam.render(world);
}