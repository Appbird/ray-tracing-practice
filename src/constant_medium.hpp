#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "rtweekend.hpp"

#include "hittable.hpp"
#include "material.hpp"
#include "texture.hpp"

class constant_medium : public hittable {
    public:
        constant_medium(
            shared_ptr<hittable> boundary,
            double density,
            shared_ptr<texture> tex
        ):
            boundary(boundary),
            neg_inv_density(-1/density),
            phase_function(make_shared<isotropic>(tex))
        {}

        constant_medium(
            shared_ptr<hittable> boundary,
            double density,
            const color& albedo
        ):
            boundary(boundary),
            neg_inv_density(-1/density),
            phase_function(make_shared<isotropic>(albedo))
        {}

        bool hit(
            const ray& r,
            interval ray_t,
            hit_record& rec
        ) const override {
            // カメラ側の衝突点と反対側の衝突点の二つを取る。
            hit_record rec1, rec2;
            if (not boundary->hit(r, interval::universe, rec1)) { return false; }
            if (not boundary->hit(r, interval{rec1.t + 0.0001, infinity}, rec2)) { return false; }

            // ray_tとの共通部分を取る
            rec1.t = std::max(rec1.t, ray_t.min);
            rec2.t = std::min(rec2.t, ray_t.max);
            if (rec1.t >= rec2.t)   { return false; }
            rec1.t = std::max(0.0, rec1.t);

            double ray_length = r.direction().length();
            double distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
            double hit_distance = neg_inv_density * std::log(random_double());

            if (hit_distance > distance_inside_boundary) { return false; }
            
            rec.t = rec1.t + hit_distance / ray_length;
            rec.p = r.at(rec.t);
            rec.normal = vec3{1, 0, 0};
            rec.front_face = true;
            rec.mat = phase_function;

            return true;
        }

        aabb bounding_box() const override { return boundary->bounding_box(); }
    private:
        shared_ptr<hittable> boundary;
        double neg_inv_density;
        shared_ptr<material> phase_function;
};
#endif