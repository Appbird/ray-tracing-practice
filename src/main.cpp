#include "rtweekend.hpp"

#include "bvh.hpp"
#include "camera.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include "material.hpp"

#include "world_setups.hpp"

#include <iostream>

int main() {
    switch (2) {
        case 1: bouncing_spheres(); break;
        case 2: checkered_spheres(); break;
        default: break;
    }
}