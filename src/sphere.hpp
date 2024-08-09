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
        aabb bbox; // bounding box
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
        {
            auto rvec = radius * vec3(1, 1, 1);
            bbox = aabb(center - rvec, center1 + rvec);
        }

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
        {
            auto rvec = radius * vec3(1, 1, 1);
            aabb box1 = aabb(center1 - rvec, center1 + rvec);
            aabb box2 = aabb(center2 - rvec, center2 + rvec);
            bbox = aabb(box1, box2);
        }
        
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
            get_sphere_uv(outward_normal, rec.u, rec.v);
            rec.mat = mat;

            return true;
        }

        aabb bounding_box() const override { return bbox; }

        /**
         * @brief Get uv coordinate in the sphere which includes `p`
         * 
         * @param p should satisfy p.length_squared() ~ 1
         * @param u output
         * @param v output
         */
        static void get_sphere_uv(
            const point3& p,
            double& u, double& v
        ) {
            assert(std::abs(p.length_squared() - 1) < 1e-9);
            double theta = std::acos(-p.y());
            double phi = std::atan2(-p.z(), p.x()) + pi;

            u = phi / (2*pi);
            v = theta / pi;
        }
};

#endif