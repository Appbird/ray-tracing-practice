#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <numbers>
#include <random>

// C++ Std Usings
using std::make_shared;
using std::shared_ptr;

// Utility Functions
constexpr float infinity = std::numeric_limits<float>::infinity();
constexpr float pi = std::numbers::pi;

// Common Headers
inline float degrees_to_radians(float degrees) {
    return degrees * pi / 180;
}
/** Returns a random real in [0, 1) */
inline float random_float() {
    static std::uniform_real_distribution<float> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);

}
inline float random_float(float min, float max) {
    return min + (max - min) * random_float();
}
inline int32_t random_int(int32_t min, int32_t max) {
    return int32_t(random_float(min, max + 1));
}

// Common Headers
#include "color.hpp"
#include "interval.hpp"
#include "ray.hpp"
#include "vec3.hpp"


#endif
