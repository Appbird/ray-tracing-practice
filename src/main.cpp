#include "rtweekend.hpp"

#include "bvh.hpp"
#include "camera.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include "quad.hpp"
#include "material.hpp"

#include "world_setups.hpp"

#include <iostream>

int main() {
    switch (5) {
        case 1: bouncing_spheres(); break;
        case 2: checkered_spheres();    break;
        case 3: earth();            break;
        case 4: perlin_spheres();   break;
        case 5: quads(); break;
        default: break;
    }
}