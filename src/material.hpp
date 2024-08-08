#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.hpp"

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

};
// ランバート反射に従うマテリアル
class lambertian : public material {
    private:
        color albedo;
    public:
    lambertian(const color& albedo) : albedo(albedo) {}

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
        attenuation = albedo;
        return true;
    }

};

// 金属マテリアル
class metal : public material {
    private:
        color albedo;
        float fuzz;
    public: 
        metal(const color& albedo, float fuzz) : albedo(albedo), fuzz(std::max(std::min(fuzz, 1.0f), 0.0f)) {};

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
        float refraction_index;
    public:
        dielectric(float refraction_index) : refraction_index(refraction_index) {}
        bool scatter(
            const ray& r_in,
            const hit_record& rec,
            color& attenuation,
            ray& scattered
        ) const override {
            attenuation = color{1.0, 1.0, 1.0};
            // 表面から入ってきたか内側から入ってきたかで屈折率が変わる
            float ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;

            const vec3 unit_direction = unit_vector(r_in.direction());
            const float cos_theta = min(1.0f, dot(-unit_direction, rec.normal));
            const float sin_theta = std::sqrt(1 - cos_theta*cos_theta);
            const bool cannot_refract = (ri*sin_theta > 1.0);
            
            vec3 next_direction;
            // Snell方程式に解が存在しなければ全反射する。
            // あるいは、そうでない場合についても一定確率（反射率）で反射させる。
            if (cannot_refract or reflectance(cos_theta, ri) > random_float()) {
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
         * @return float 反射率
         */
        static float reflectance(
            const float cosine,
            const float refraction_index
        ) {
            float r0 = (1 - refraction_index) / (1 + refraction_index);
            r0 = r0 * r0;
            return r0 + (1 - r0) * std::pow(1 - cosine, 5);
        }
};

#endif