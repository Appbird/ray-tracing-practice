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

class rotate_y : public hittable {
    public:
        rotate_y(
            shared_ptr<hittable> object,
            double angle
        ): object(object) {
            double radians = degrees_to_radians(angle);
            sin_theta = std::sin(radians);
            cos_theta = std::cos(radians);
            bbox = object->bounding_box();
            
            point3 min{infinity, infinity, infinity};
            point3 max{-infinity, -infinity, -infinity};

            for (int32_t i = 0; i < 2; i++) {
                for (int32_t j = 0; j < 2; j++) {
                    for (int32_t k = 0; k < 2; k++) {
                        double x = (i == 0) ? bbox.x.max : bbox.x.min;
                        double y = (j == 0) ? bbox.y.max : bbox.y.min;
                        double z = (k == 0) ? bbox.z.max : bbox.z.min;
                        vec3 vertex = rotate_vector_positive({x, y, z});

                        for (int32_t l = 0; l < 3; l++) {
                            min[l] = std::min(min[l], vertex[l]);
                            max[l] = std::max(min[l], vertex[l]);
                        }
                    }
                }
            }
            

        }
        bool hit(
            const ray& r,
            interval ray_t,
            hit_record& rec
        ) const override {
            vec3 origin     = rotate_vector_negative(r.origin());
            vec3 direction  = rotate_vector_negative(r.direction());
            // Y軸で回転
            ray rotated_r{origin, direction, r.time()};

            if (not object->hit(rotated_r, ray_t, rec)) { return false; }
            rec.p = rotate_vector_positive(rec.p);
            rec.normal = rotate_vector_positive(rec.normal);
            return true;
        }
        aabb bounding_box() const override { return bbox; }
        
    private:
        shared_ptr<hittable> object;
        aabb bbox;
        double cos_theta;
        double sin_theta;
        // θ負の方向にベクトルを回転させる（x,y,zが右手座標系をとっている）
        vec3 rotate_vector_negative(const vec3& p) const {
            vec3 result = p;
            result[0] = cos_theta*p[0] - sin_theta*p[2];
            result[2] = sin_theta*p[0] + cos_theta*p[2];
            return result;
        }
        // θ正の方向にベクトルを回転させる（x,y,zが右手座標系をとっている）
        vec3 rotate_vector_positive(const vec3& p) const {
            vec3 result = p;
            result[0] = cos_theta*p[0] + sin_theta*p[2];
            result[2] = -sin_theta*p[0] + cos_theta*p[2];
            return result;
        }
};

#endif