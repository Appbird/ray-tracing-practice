#ifndef TEXTURE_H
#define TEXTURE_H

#include "rtweekend.hpp"
#include "perlin.hpp"
#include "external/rtw_stb_image.hpp"

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

class image_texture : public texture {
    public:
        image_texture(const char* filename) : image(filename) {}
        color value(
            double u,
            double v,
            [[maybe_unused]] const point3& p
        ) const override {
            if (image.height() <= 0) { return color{1, 0, 1}; }

            u = interval{0, 1}.clamp(u);
            v = 1.0 - interval{0, 1}.clamp(v);
            
            auto i = int32_t(u * image.width());
            auto j = int32_t(v * image.height());
            auto pixel = image.pixel_data(i, j);

            auto color_scale = 1.0 / 255.0;
            return color{color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]};
        }
    private:
        rtw_image image;
};

class noise_texture : public texture {
    public:
        noise_texture(double scale) : scale(scale) {}
        color value(
            [[maybe_unused]] double u,
            [[maybe_unused]] double v,
            const point3& p
        ) const override {
            // return color{1, 1, 1} * (0.5 * (noise.noise(p * scale) + 1.0));
            // return color{1, 1, 1} * noise.turb(p, 7);
            return color{0.5,0.5,0.5} * (1 + std::sin(scale * p.z() + 10 * noise.turb(p, 7)));
        }
    private:
        perlin noise;
        double scale;
};


#endif