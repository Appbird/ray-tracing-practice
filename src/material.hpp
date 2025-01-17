#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.hpp"

#include "texture.hpp"

using std::min;
using std::max;

class material {
    public:
    virtual ~material() = default;

    /**
     * @brief 入射するレイ`r_in`に対して反射光`scattered`を計算します。
     * 
     * @param r_in 
     * @param rec 衝突点に関する情報
     * @param attenuation RGB色値別の反射率（たとえば、R値が0.5であれば、赤色の光に関して0.5倍の強さを持つ光を反射させることになります。）
     * @param scattered 
     * @return true 光が反射します。
     * @return false 
     */
    virtual bool scatter(
        [[maybe_unused]] const ray& r_in,
        [[maybe_unused]] const hit_record& rec,
        [[maybe_unused]] color& attenuation,
        [[maybe_unused]] ray& scattered
    ) const {
        return false;
    }

    virtual color emitted(
        [[maybe_unused]] double u,
        [[maybe_unused]] double v,
        [[maybe_unused]] const point3& p
    ) const {
        return color{0, 0, 0};
    }

};
// ランバート反射に従うマテリアル
class lambertian : public material {
    public:
    lambertian(const color& albedo) : tex(make_shared<solid_color>(albedo)) {}
    lambertian(shared_ptr<texture> tex) : tex(tex) {}
    bool scatter(
        const ray& r_in,
        const hit_record& rec,
        color& attenuation,
        ray& scattered
    ) const override {
        vec3 scatter_direction = rec.normal + random_unit_vector();
        if (scatter_direction.near_zero()) {
            scatter_direction = rec.normal;
        }

        scattered = ray{rec.p, scatter_direction, r_in.time()};
        attenuation = tex->value(rec.u, rec.v, rec.p);
        return true;
    }
    private:
        shared_ptr<texture> tex;
};

// 金属マテリアル
class metal : public material {
    private:
        color albedo;
        double fuzz;
    public: 
        metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(std::max(std::min(fuzz, 1.0), 0.0)) {};

        bool scatter(
            const ray& r_in,
            const hit_record& rec,
            color& attenuation,
            ray& scattered
        ) const override {
            vec3 reflected = reflect(r_in.direction(), rec.normal);
            reflected = unit_vector(reflected) + (fuzz * random_in_unit_sphere());
            scattered = ray{rec.p, reflected, r_in.time()};
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }
};

// 絶縁体
class dielectric : public material { 
    private:
        /**
         * Refractive index in vacuum or air,
         * or the ratio of the material's refractive index over
         * the refractive index of the enclosing media
         */
        double refraction_index;
    public:
        dielectric(double refraction_index) : refraction_index(refraction_index) {}
        bool scatter(
            const ray& r_in,
            const hit_record& rec,
            color& attenuation,
            ray& scattered
        ) const override {
            attenuation = color{1.0, 1.0, 1.0};
            // 表面から入ってきたか内側から入ってきたかで屈折率が変わる
            double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;

            const vec3 unit_direction = unit_vector(r_in.direction());
            const double cos_theta = min(1.0, dot(-unit_direction, rec.normal));
            const double sin_theta = std::sqrt(1 - cos_theta*cos_theta);
            const bool cannot_refract = (ri*sin_theta > 1.0);
            
            vec3 next_direction;
            // Snell方程式に解が存在しなければ全反射する。
            // あるいは、そうでない場合についても一定確率（反射率）で反射させる。
            if (cannot_refract or reflectance(cos_theta, ri) > random_double()) {
                next_direction = reflect(unit_direction, rec.normal);
            } else {
                next_direction = reflact(unit_direction, rec.normal, ri);
            }
            scattered = ray{rec.p, next_direction, r_in.time()};
            return true;
        }
        /**
         * @brief Schlickの近似式を用いて反射係数（反射率）を求める
         * 
         * @param cosine 入射角のコサイン値
         * @param refraction_index 屈折率
         * @return double 反射率
         */
        static double reflectance(
            const double cosine,
            const double refraction_index
        ) {
            double r0 = (1 - refraction_index) / (1 + refraction_index);
            r0 = r0 * r0;
            return r0 + (1 - r0) * std::pow(1 - cosine, 5);
        }
};

class diffuse_light : public material {
    public: 
        diffuse_light(shared_ptr<texture> tex) : tex(tex) {}
        diffuse_light(const color& emit): tex(make_shared<solid_color>(emit)) {}
        color emitted(double u, double v, const point3& p) const override {
            return tex->value(u, v, p);
        }
    private:
        shared_ptr<texture> tex;
};


class isotropic : public material {
    public:
        isotropic(const color& albedo) : tex(make_shared<solid_color>(albedo)) {}
        isotropic(shared_ptr<texture> tex): tex(tex) {}
        bool scatter(
            const ray& r_in,
            const hit_record& rec,
            color& attenuation,
            ray& scattered
        ) const override {
            scattered = ray{rec.p, random_unit_vector(), r_in.time()};
            attenuation = tex->value(rec.u, rec.v, rec.p);
            return true;
        }
    private:
        shared_ptr<texture> tex;
 };

 
#endif