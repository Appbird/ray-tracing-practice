#ifndef VEC3_H
#define VEC3_H

#include "rtweekend.hpp"

#include <cmath>
#include <iostream>

class vec3 {
  public:
    double e[3];

    vec3() : e{0,0,0} {}
    vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    vec3& operator+=(const vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    vec3& operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3& operator/=(double t) {
        return *this *= 1/t;
    }

    double length() const {
        return std::sqrt(length_squared());
    }

    double length_squared() const {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }

    static vec3 random() {
        return vec3{
            random_double(),
            random_double(),
            random_double()
        };
    }
    static vec3 random(double min, double max) {
        return vec3{
            random_double(min, max),
            random_double(min, max),
            random_double(min, max),
        };
    }
    bool near_zero() {
        double s = 1e-8;
        return 
            (std::abs(e[0]) < s) and
            (std::abs(e[1]) < s) and
            (std::abs(e[2]) < s);
    }
};

// point3 is just an alias for vec3, but useful for geometric clarity in the code.
using point3 = vec3;


// Vector Utility Functions

inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3& v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3& v, double t) {
    return t * v;
}

inline vec3 operator/(const vec3& v, double t) {
    return (1/t) * v;
}

inline double dot(const vec3& u, const vec3& v) {
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(const vec3& v) {
    return v / v.length();
}

inline vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3{random_double(-1, 1), random_double(-1, 1), 0};
        if (p.length_squared() < 1) { return p; }
    }
}

inline vec3 random_in_unit_sphere() {
    while (true) {
        vec3 p = vec3::random(-1, 1);
        if (p.length_squared() < 1) { return p; }
    }
}
inline vec3 random_unit_vector() {
    return unit_vector(random_in_unit_sphere());
}
inline vec3 random_on_hemisphere(const vec3& normal) {
    const vec3 on_unit_sphere = random_unit_vector();
    return (dot(on_unit_sphere, normal) > 0) ? on_unit_sphere : -on_unit_sphere;
}

inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2*dot(v, n)*n;
}
/**
 * @brief 法線`n`をもつ面に入射する光`uv`の反射光を求める。
 * 
 * @param uv 入射光
 * @param n 入射する面の法線
 * @param etai_over_etat 屈折率
 * @return vec3 
 */
inline vec3 reflact(
    const vec3& uv, 
    const vec3& n,
    double etai_over_etat
) {
    // `theta` : 入射光と法線がなす角
    const double cos_theta = std::min(dot(-uv, n), 1.0);

    const vec3 r_out_prep = etai_over_etat * (uv + (cos_theta)*n);
    const vec3 r_out_parallel = -std::sqrt(std::abs(1.0 - r_out_prep.length_squared())) * n;
    return r_out_prep + r_out_parallel;
}

#endif