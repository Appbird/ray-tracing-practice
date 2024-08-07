#include "rtweekend.hpp"

#include "camera.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include "material.hpp"

#include <iostream>

hittable_list world_setup1() {
    // World
    hittable_list world;

    //material
    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left   = make_shared<dielectric>(1.50 / 1.00);
    auto material_bubble   = make_shared<dielectric>(1.00 / 1.50);
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

    world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.2),   0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.4, material_bubble));
    world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));
    return world;
}

hittable_list world_setup_fov_test() {
    hittable_list world;

    double R = std::cos(pi / 4);
    auto material_left = make_shared<lambertian>(color{0, 0, 1});
    auto material_right = make_shared<lambertian>(color{1, 0, 0});

    world.add(make_shared<sphere>(point3{-R, 0, -1}, R, material_left));
    world.add(make_shared<sphere>(point3{R, 0, -1}, R, material_right));

    return world;
}

int main() {
    hittable_list world = world_setup1();

    camera cam;
    cam.aspect_ratio    = 16.0 / 9.0;
    cam.image_width     = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = {-2, 2, 1};
    cam.lookat   = {0, 0, -1};
    cam.vup      = {0, 1, 0};

    cam.defocus_angle = 10.0;
    cam.focus_dist = 3.4;

    cam.render(world);
}