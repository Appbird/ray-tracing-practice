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
constexpr double infinity = std::numeric_limits<double>::infinity();
constexpr double pi = std::numbers::pi;

// Common Headers
inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180;
}
/** Returns a random real in [0, 1) */
inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);

}
inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}
inline int32_t random_int(int32_t min, int32_t max) {
    return int32_t(random_double(min, max + 1));
}

// Common Headers
#include "color.hpp"
#include "interval.hpp"
#include "ray.hpp"
#include "vec3.hpp"


#endif
