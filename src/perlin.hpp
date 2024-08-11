#ifndef PERLIN_H
#define PERLIN_H

#include "rtweekend.hpp"
#include <array>

#define rep(i, n) for (int32_t i = 0; i < n; i++)

class perlin {
    public:
        perlin() {
            for (int32_t i = 0; i < point_count; i++) {
                randvec[i] = unit_vector(vec3::random(-1, 1));
            }
            perlin_generate_perm(perm_x);
            perlin_generate_perm(perm_y);
            perlin_generate_perm(perm_z);
        }
        double noise(const point3& p) const {
            // pの各座標値の小数部分
            double u = p.x() - std::floor(p.x());
            double v = p.y() - std::floor(p.y());
            double w = p.z() - std::floor(p.z());
            // pの各座標値の整数部分
            auto i = int32_t(std::floor(p.x()));
            auto j = int32_t(std::floor(p.y()));
            auto k = int32_t(std::floor(p.z()));
            vec3 c[2][2][2];
            rep(di, 2) {
                rep(dj, 2) {
                    rep(dk, 2) {
                        c[di][dj][dk] = randvec[
                            perm_x[(i + di) & 0xff] ^
                            perm_y[(j + dj) & 0xff] ^
                            perm_z[(k + dk) & 0xff]
                        ];
                    }
                }
            }
            
            return perlin_interp(c, u, v, w);
        }
        /** 乱流ノイズの点`p`の値を返す。 */
        double turb(const point3& p, int32_t depth) const {
            double accum = 0.0;
            point3 temp_p = p;
            auto weight = 1.0;
            for (int32_t i = 0; i < depth; i++) {
                accum += weight * noise(temp_p);
                weight *= 0.5;
                temp_p *= 2;
            }
            return std::abs(accum);
        }
    private:
        static constexpr int32_t point_count = 256;
        std::array<vec3, point_count> randvec;
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
        static double perlin_interp(
            vec3 c[2][2][2],
            double u, double v, double w
        ) {
            double uu = hermite(u);
            double vv = hermite(v);
            double ww = hermite(w);

            double accum = 0.0;
            rep(i, 2) {
                rep(j, 2) {
                    rep (k, 2) {
                        vec3 weight_v(u-i, v-j, w-k);
                        accum += 
                            (i*uu + (1 - i)*(1 - uu)) *
                            (j*vv + (1 - j)*(1 - vv)) *
                            (k*ww + (1 - k)*(1 - ww)) *
                            dot(c[i][j][k], weight_v);
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