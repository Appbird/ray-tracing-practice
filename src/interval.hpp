#ifndef INTERVAL_H
#define INTERVAL_H
#include "rtweekend.hpp"

struct interval {
    double min, max;

    // Default interval is empty.
    interval(): min(+infinity), max(-infinity) {}

    interval(double min, double max): min(min), max(max) {}
    // `a`, `b`の共通部分
    interval(const interval& a, const interval& b) {
        min = std::min(a.min, b.min);
        max = std::max(a.max, b.max);
    }

    double size() const { return max - min; }
    double contains(double x) const     { return min <= x and x <= max; }
    double surrounds(double x) const    { return min < x and x < max; }
    
    double clamp(double x) const {
        if (x < min) { return min; }
        if (x > max) { return max; }
        return x;
    }
    interval expand(double delta) const {
        double padding = delta / 2;
        return interval{ min - padding, max + padding };
    }

    double overlaps(interval i1, interval i2) {
        double t_min = std::max(i1.min, i2.min);
        double t_max = std::min(i1.max, i2.max);
        return t_min < t_max;
    }

    bool is_empty() const {
        return max <= min;
    }

    static const interval empty, universe;
};

const interval interval::empty      = interval(+infinity, -infinity);
const interval interval::universe   = interval(-infinity, +infinity);

interval operator+(const interval& ival, double displacement) {
    return interval{ival.min + displacement, ival.max + displacement};
}
interval operator+(double displacement, const interval& ival) {
    return ival + displacement;
}

#endif