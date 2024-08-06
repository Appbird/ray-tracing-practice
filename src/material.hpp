#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.hpp"

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
        const ray& r_in,
        const hit_record& rec,
        color& attenuation,
        ray& scattered
    ) const {
        return false;
    }

};

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

        scattered = ray{rec.p, scatter_direction};
        attenuation = albedo;
        return true;
    }

};

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
            scattered = ray(rec.p, reflected);
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }
};

#endif