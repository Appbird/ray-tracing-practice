#ifndef INTERVAL_H
#define INTERVAL_H
#include "rtweekend.hpp"

struct interval {
    float min, max;

    // Default interval is empty.
    interval(): min(+infinity), max(-infinity) {}

    interval(float min, float max): min(min), max(max) {}
    // `a`, `b`の共通部分
    interval(const interval& a, const interval& b) {
        min = std::min(a.min, b.min);
        max = std::max(a.max, b.max);
    }

    float size() const { return max - min; }
    float contains(float x) const     { return min <= x and x <= max; }
    float surrounds(float x) const    { return min < x and x < max; }
    
    float clamp(float x) const {
        if (x < min) { return min; }
        if (x > max) { return max; }
        return x;
    }
    interval expand(float delta) const {
        float padding = delta / 2;
        return interval{ min - padding, max + padding };
    }

    float overlaps(interval i1, interval i2) {
        float t_min = std::max(i1.min, i2.min);
        float t_max = std::min(i1.max, i2.max);
        return t_min < t_max;
    }

    bool is_empty() const {
        return max <= min;
    }

    static const interval empty, universe;
};

const interval interval::empty      = interval(+infinity, -infinity);
const interval interval::universe   = interval(-infinity, +infinity);

#endif