#ifndef AABB_H
#define AABB_H

#include "rtweekend.hpp"
#include <cassert>

class aabb {
    public:
        interval x, y, z;
        aabb() {}
        aabb(
            const interval& x,
            const interval& y,
            const interval& z
        ) : x(x), y(y), z(z) {}
        // Treat the two points a and b as extrema for the bounding box, so we don't require a
        // particular minimum/maximum coordinate order.
        aabb(
            const point3& a, 
            const point3& b
        ) {
            x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
            y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
            z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);
        }
        const interval& axis_interval(int n) {
            switch (n) {
                case 1: return y;
                case 2: return z;
                default: return x;
            }
        }

        bool hit(const ray& r, interval ray_t) {
            const point3& ray_orig  = r.origin();
            const point3& ray_dir   = r.direction();
            
            for (int32_t axis = 0; axis < 3; axis++) {
                const interval& ax = axis_interval(axis);
                double t0 = (ax.min - ray_orig[axis]) / ray_dir[axis];
                double t1 = (ax.max - ray_orig[axis]) / ray_dir[axis];
                if (t0 > t1) { std::swap(t0, t1); }
                assert(t0 <= t1);

                if (t0 > ray_t.min) { ray_t.min = t0; }
                if (t1 < ray_t.max) { ray_t.max = t1; }

                if (ray_t.is_empty()) { return false; }
            }
            return true;
        }
};

#endif