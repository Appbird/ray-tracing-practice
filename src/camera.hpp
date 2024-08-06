#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.hpp"
#include "hittable.hpp"

class camera {
    public:
    double aspect_ratio = 1.0;
    int image_width = 100;
    int samples_per_pixel = 10;

    public:
    void render(const hittable& world) {
        initialize();
        // Write ppm header
        std::cout << "P3" << std::endl;
        std::cout << image_width << " " << image_height << std::endl;
        std::cout << 255 << std::endl;

        // Rendering
        for (int32_t j = 0; j < image_height; j++) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << " " << std::flush;
            for (int32_t i = 0; i < image_width; i++) {
                color pixel_color{0, 0, 0};
                // 複数点をサンプリングしてレイを飛ばした上で、その色の平均を最終出力結果とする。
                for (int32_t sample = 0; sample < samples_per_pixel; sample++) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, world);
                }
                write_color(std::cout, pixel_samples_scale * pixel_color);
            }
        }
        std::clog << "\rDone.                       \n" << std::flush;
    }

    private:
    /** Rendered image height */
    int32_t image_height;
    /** Color scale factor for a sum of pixel samples */
    double pixel_samples_scale;
    /** Camera Center */
    point3 center;
    /** Location of pixel 0, 0 */
    point3 pixel00_loc;

    /** Offset to pixel to the right */
    vec3 pixel_delta_u;
    /** Offset to pixel below */
    vec3 pixel_delta_v;

    void initialize() {
        // Output Image Settings
        image_height = std::max(int32_t(image_width / aspect_ratio), 1);
        
        pixel_samples_scale = 1.0 / samples_per_pixel;

        // Viewport Settings
        double focal_length = 1.0;
        double viewport_height = 2.0;
        double viewport_width = viewport_height * (double(image_width) / image_height);
        center = {0, 0, 0};

        // vector related to viewport 
        // --------------------------
        /** vector across the vertical viewport_edge */
        const vec3 viewport_u = {viewport_width, 0, 0};
        /** vector across the horizontal viewport_edge */
        const vec3 viewport_v = {0, -viewport_height, 0};
        
        /** vector across the vertical edge of pixel in viewport */
        pixel_delta_u = viewport_u / image_width;
        /** vector across the horizontal edge of pixel in viewport */
        pixel_delta_v = viewport_v / image_height;

        const vec3 viewport_center = center - vec3{0, 0, focal_length};
        const vec3 viewport_upper_left = viewport_center - (viewport_u + viewport_v)/2;
        pixel00_loc = viewport_upper_left + (pixel_delta_u + pixel_delta_v)/2;
        // --------------------------
    }
    
    ray get_ray(int32_t i, int32_t j) const {
        const vec3 offset = sample_square();
        const vec3 pixel_sample = pixel00_loc
            + ((i + offset.x()) * pixel_delta_u)
            + ((j + offset.y()) * pixel_delta_v);
        
        const vec3 ray_origin = center;
        const vec3 ray_direction = pixel_sample - ray_origin;

        return ray{ray_origin, ray_direction};
    }

    vec3 sample_square() const {
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }
 
    /**
     * @brief `world`に向けて飛ばした飛ばした光線`r`が何色かを評価する。
     */
    color ray_color(
        const ray& r,
        const hittable& world
    ) const {
        // Hittableに衝突したときの、その位置に関する情報
        hit_record rec;
        // 物体に衝突した場合には
        // その衝突点からさらにランダムな方向にレイを飛ばし、その飛ばしたレイの色を用いて評価する
        // 光線の逆進性を用いてこれを解釈すると、ある方向から飛んできたレイが反射率の影響を受けながらランダムな方向に飛んでいく過程だとみなせる。
        if (world.hit(r, interval{0, infinity}, rec)) {
            vec3 direction = random_on_hemisphere(rec.normal);
            return 0.5 * ray_color(ray{rec.p, direction}, world);
        }
        // 無限遠にレイが飛んでいくようであれば、空の色を返す。
        vec3 unit_direction = unit_vector(r.direction());
        const double a = 0.5 * (unit_direction.y() + 1.0);
        return (1 - a)*color{1.0, 1.0, 1.0} + a*color{0.5, 0.7, 1.0};
    }

};

#endif