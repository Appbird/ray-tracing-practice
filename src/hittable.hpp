#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.hpp"
#include "aabb.hpp"
#include <cassert>

class material;

class hit_record {
    public:
        point3 p;
        vec3 normal;
        shared_ptr<material> mat;
        double t;
        double u;
        double v;
        bool front_face;

        void set_face_normal(
            const ray& r,
            const vec3& outward_normal
        ) {
            assert(std::abs(outward_normal.length_squared() - 1) < 1e-2);
            front_face = (dot(r.direction(), outward_normal) < 0);
            normal = front_face ? outward_normal : -outward_normal;
        }
};

class hittable {
    public:
        virtual ~hittable() = default;
        virtual bool hit(
            const ray& r,
            interval ray_t,
            hit_record& rec
        ) const = 0;
        virtual aabb bounding_box() const = 0;
};

class translate : public hittable {
    public:
        translate(
            shared_ptr<hittable> object,
            const vec3& offset
        ) : object(object), offset(offset) {
            bbox = object->bounding_box() + offset;
        }
        bool hit(
            const ray& r,
            interval ray_t,
            hit_record& rec
        ) const override {
            ray offset_r{
                r.origin() - offset,
                r.direction(),
                r.time()
            };
            if (not object->hit(offset_r, ray_t, rec)) { return false; }
            rec.p += offset;
            return true;
        }
        aabb bounding_box() const override { return bbox; }
    private:
        shared_ptr<hittable> object;
        vec3 offset;
        aabb bbox;
};

#endif