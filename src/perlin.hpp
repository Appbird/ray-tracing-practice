#ifndef PERLIN_H
#define PERLIN_H

#include "rtweekend.hpp"
#include <array>

#define rep(i, n) for (int32_t i = 0; i < n; i++)

class perlin {
    public:
        perlin() {
            for (int32_t i = 0; i < point_count; i++) {
                randfloat[i] = random_double();
            }
            perlin_generate_perm(perm_x);
            perlin_generate_perm(perm_y);
            perlin_generate_perm(perm_z);
        }
        double noise(const point3& p) const {
            double u = p.x() - std::floor(p.x());
            double v = p.y() - std::floor(p.y());
            double w = p.z() - std::floor(p.z());
            u = hermite(u);
            v = hermite(v);
            w = hermite(w);
            auto i = int32_t(std::floor(p.x()));
            auto j = int32_t(std::floor(p.y()));
            auto k = int32_t(std::floor(p.z()));
            double c[2][2][2];
            rep(di, 2) {
                rep(dj, 2) {
                    rep(dk, 2) {
                        c[di][dj][dk] = randfloat[
                            perm_x[(i + di) & 0xff] ^
                            perm_y[(j + dj) & 0xff] ^
                            perm_z[(k + dk) & 0xff]
                        ];
                    }
                }
            }
            
            return trilinear_interp(c, u, v, w);
        }
    private:
        static constexpr int32_t point_count = 256;
        std::array<double, point_count> randfloat;
        std::array<int32_t, point_count> perm_x;
        std::array<int32_t, point_count> perm_y;
        std::array<int32_t, point_count> perm_z;

        static void perlin_generate_perm(std::array<int32_t, point_count>& p) {
            for (int32_t i = 0; i < point_count; i++) {
                p[i] = i;
            }
            permute(p, point_count);
        }

        static void permute(std::array<int32_t, point_count>& p, int32_t n) {
            for (int32_t i = n - 1; i > 0; i--) {
                int32_t target = random_int(0, i);
                std::swap(p[i], p[target]);
            }
        }

        static double trilinear_interp(
            double c[2][2][2],
            double u, double v, double w
        ) {
            double accum = 0.0;
            rep(i, 2) {
                rep(j, 2) {
                    rep (k, 2) {
                        accum += 
                            (i*u + (1 - i)*(1 - u)) *
                            (j*v + (1 - j)*(1 - v)) *
                            (k*w + (1 - k)*(1 - w)) *
                            c[i][j][k];
                    }
                }
            }
            return accum;
        }
        static inline double hermite(double x) {
            return x*x*(3-2*x);
        }
};

#endif