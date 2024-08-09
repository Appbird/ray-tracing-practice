#ifndef TEXTURE_H
#define TEXTURE_H

#include "rtweekend.hpp"

class texture {
    public:
    virtual ~texture() = default;
    virtual color value(
        double u,
        double v,
        const point3& p
    ) const = 0;
};

class solid_color : public texture {
    public:
        solid_color(const color& albedo) : albedo(albedo) {}
        solid_color(double red, double green, double blue) :
            solid_color(color{red, green, blue})
        {}

        color value(
            [[maybe_unused]] double u,
            [[maybe_unused]] double v,
            [[maybe_unused]] const point3& p
        ) const override {
            return albedo;
        }
    private:
        color albedo;
};

class checker_texture : public texture {
    public:
        checker_texture(
            double scale,
            shared_ptr<texture> even,
            shared_ptr<texture> odd
        ):
            inv_scale(1.0 / scale),
            even(even), odd(odd)
        {}
        checker_texture(double scale, const color& c1, const color& c2) :
            checker_texture(
                scale,
                make_shared<solid_color>(c1),
                make_shared<solid_color>(c2)
            )
        {}

        color value(
            double u,
            double v,
            const point3& p
        ) const override {
            auto x_integer = int(std::floor(inv_scale * p.x()));
            auto y_integer = int(std::floor(inv_scale * p.y()));
            auto z_integer = int(std::floor(inv_scale * p.z()));

            bool is_even = (x_integer + y_integer + z_integer) % 2 == 0;
            return is_even ? even->value(u, v, p) : odd->value(u, v, p);
        }
    private:
        double inv_scale;
        shared_ptr<texture> even;
        shared_ptr<texture> odd;
};


#endif