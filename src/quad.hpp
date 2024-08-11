#ifndef QUAD_H
#define QUAD_H

#include "rtweekend.hpp"
#include "material.hpp"
#include "aabb.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"

class plane_figure : public hittable {
    public:
        plane_figure(
            const vec3& Q,
            const vec3& u,
            const vec3& v,
            shared_ptr<material> mat
        ):
            Q(Q), u(u), v(v), mat(mat)
        {
            vec3 n = cross(u, v);
            normal = unit_vector(n);
            D = dot(normal, Q);
            w = n / dot(n, n);
            set_bounding_box();
        }

        void set_bounding_box() {
            aabb box_diagnoal1 = {Q, Q + u + v};
            aabb box_diagnoal2 = {Q + u, Q + v};
            bbox = aabb{box_diagnoal1, box_diagnoal2};
        }
        aabb bounding_box() const override { return bbox; }
        
        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            double denom = dot(normal, r.direction());
            if (std::abs(denom) < 1e-8) { return false; }

            double t = (D - dot(normal, r.origin())) / denom;
            if (not ray_t.contains(t)) { return false; }

            point3 intersection = r.at(t);
            vec3 planar_hitpt_vector = intersection - Q;
            double alpha = dot(w, cross(planar_hitpt_vector, v));
            double beta  = dot(w, cross(u, planar_hitpt_vector));

            if (not is_interior(alpha, beta, rec)) { return false; }

            rec.t = t;
            rec.p = intersection;
            rec.mat = mat;
            rec.set_face_normal(r, normal);

            return true;
        }
        virtual bool is_interior(double a, double b, hit_record& rec) const = 0;
    protected:
        // Qを起点として、uとvによって張られた空間を想定する。
        vec3 Q, u, v;
        shared_ptr<material> mat;
        aabb bbox;
        vec3 normal;
        vec3 w;
        double D;
};

class quad : public plane_figure {
    public:
        quad(
            const vec3& Q,
            const vec3& u,
            const vec3& v,
            shared_ptr<material> mat
        ): plane_figure(Q, u, v, mat) {
            set_bounding_box();
        }
        
        
        bool is_interior(double a, double b, hit_record& rec) const override {
            interval unit_interval = interval{0, 1};
            if (unit_interval.contains(a) and unit_interval.contains(b)) {
                rec.u = a;
                rec.v = b;
                return true;
            } else {
                return false;
            }
        }
};

class disk : public plane_figure {
    public:
        disk(
            const vec3& Q,
            const vec3& u,
            const vec3& v,
            shared_ptr<material> mat,
            double r
        ): plane_figure(Q, u, v, mat), r(r)
        {
            set_bounding_box();
        }
        
        bool is_interior(double a, double b, hit_record& rec) const override {
            a = (a - 0.5) * 2;
            b = (b - 0.5) * 2;
            if (a*a + b*b < r*r) {
                rec.u = (a - 0.5) *2;
                rec.v = (b - 0.5) *2;
                return true;
            } else {
                return false;
            }
        }
    private:
        double r;
};

class triangle : public plane_figure {
    public:
        triangle(
            const vec3& Q,
            const vec3& u,
            const vec3& v,
            shared_ptr<material> mat
        ): plane_figure(Q, u, v, mat) {}
        
        bool is_interior(double a, double b, hit_record& rec) const override {
            if (0 < a and 0 < b and a + b < 1) {
                rec.u = a;
                rec.v = b;
                return true;
            } else {
                return false;
            }
        }
};
class ring : public plane_figure {
    public:
        ring(
            const vec3& Q,
            const vec3& u,
            const vec3& v,
            shared_ptr<material> mat,
            double r_out,
            double r_in
        ): plane_figure(Q, u, v, mat), r_out(r_out), r_in(r_in)
        {}
        
        bool is_interior(double a, double b, hit_record& rec) const override {
            a = (a - 0.5) * 2;
            b = (b - 0.5) * 2;
            double r_sq = a*a + b*b;
            if (r_in*r_in < r_sq and r_sq < r_out*r_out) {
                rec.u = (a - 0.5) *2;
                rec.v = (b - 0.5) *2;
                return true;
            } else {
                return false;
            }
        }
    private:
        double r_out;
        double r_in;
};

inline shared_ptr<hittable_list>box(
    const point3& a, 
    const point3& b,
    shared_ptr<material> mat
) {
    auto sides = make_shared<hittable_list>();

    // 真反対の位置にあるボックス
    auto min = point3{
        std::min(a.x(), b.x()),
        std::min(a.y(), b.y()),
        std::min(a.z(), b.z())
    };
    auto max = point3{
        std::max(a.x(), b.x()),
        std::max(a.y(), b.y()),
        std::max(a.z(), b.z())
    };

    vec3 dx = (max.x() - min.x()) * vec3{1, 0, 0};
    vec3 dy = (max.y() - min.y()) * vec3{0, 1, 0};
    vec3 dz = (max.z() - min.z()) * vec3{0, 0, 1};

    sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()), dx, dy, mat));
    sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz, dy, mat));
    sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx, dy, mat));
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dz, dy, mat));
    sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()), dx, -dz, mat));
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dx, dz, mat));
    return sides;
}


#endif