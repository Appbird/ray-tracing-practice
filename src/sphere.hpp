#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.hpp"

class sphere : public hittable {
    private:
        point3 center1;
        double radius;
        std::shared_ptr<material> mat;
        bool is_moving;
        vec3 center_vec;
    public:
        sphere(
            const point3& center,
            const double radius,
            shared_ptr<material> mat
        ):
            center1(center),
            radius(std::max(radius, 0.0)),
            mat(mat),
            is_moving(false)
        {}

        sphere(
            const point3& center1,
            const point3& center2,
            double radius,
            shared_ptr<material> mat
        ):
            center1(center1),
            radius(radius),
            mat(mat),
            is_moving(true),
            center_vec(center2 - center1)
        {}
        
        point3 sphere_center(double time) const {
            return center1 + time * center_vec;
        }

        bool hit(
            const ray& r,
            interval ray_t,
            hit_record& rec
        ) const override
        {
            const point3 center = is_moving ? sphere_center(r.time()) : center1;
            const vec3 oc = center - r.origin();
            const double a = r.direction().length_squared();
            const double b = dot(r.direction(), oc);
            const double c = oc.length_squared() - radius*radius;
            const double discriminant = b*b - a*c;
            
            if (discriminant < 0) { return false; }

            const double sqrt_d = std::sqrt(discriminant);
            double root = (b - sqrt_d) / a;
            if (root <= ray_t.min) { root = (b + sqrt_d) / a; }
            if (root <= ray_t.min or ray_t.max <= root) { return false; }

            rec.t = root;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat = mat;

            return true;
        }

};

#endif