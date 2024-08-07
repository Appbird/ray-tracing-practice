#ifndef WORLD_SETUPS_H
#define WORLD_SETUPS_H

#include "rtweekend.hpp"

#include "hittable.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include "material.hpp"

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

#endif