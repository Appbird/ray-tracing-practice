#ifndef INTERVAL_H
#define INTERVAL_H
#include "rtweekend.hpp"

struct interval {
    double min, max;

    // Default interval is empty.
    interval(): min(+infinity), max(-infinity) {}

    interval(double min, double max): min(min), max(max) {}

    double size() const { return max - min; }
    double contains(double x) const     { return min <= x and x <= max; }
    double surrounds(double x) const    { return min < x and x < max; }
    
    double clamp(double x) const {
        if (x < min) { return min; }
        if (x > max) { return max; }
        return x;
    }

    static const interval empty, universe;
};

const interval interval::empty      = interval(+infinity, -infinity);
const interval interval::universe   = interval(-infinity, +infinity);

#endif