#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.hpp"
#include "hittable.hpp"
#include "material.hpp"

/**
 * @brief 与えられたワールドの特定の位置からレイを発射し、それらの色を評価することで色を定める。
 * 
 */
class camera {
    public:
    // アスペクト比
    double aspect_ratio = 1.0;
    // 画像のピクセルの幅
    int32_t image_width = 100;
    // 1ピクセルあたり何回レイを飛ばすか
    int32_t samples_per_pixel = 10;
    // 反射回数
    int32_t max_depth = 10;
    color background;
    
    /** Vertical view angle (field of view) */
    double vfov = 90;
    // カメラの視点
    point3 lookfrom = {0, 0, 0};
    // カメラの注視点
    point3 lookat   = {0, 0, -1};
    // カメラの上側
    point3 vup      = {0, 1, 0};

    /** ピクセルを通過するレイの角度の最大角 */
    double defocus_angle = 0;
    /** カメラの視点lookfromから焦点が完璧に写る平面までの距離 */
    double focus_dist = 10;

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
                    pixel_color += ray_color(r, world, max_depth - 1);
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

    // Camera frame normalized orthogonal basis vectors
    vec3 u; // the unit vector pointing to camera right
    vec3 v; // the unit vector pointing to camera up
    vec3 w; // the unit vector pointing opposite the view direction

    // Defocus disk
    vec3 defocus_disk_u; // Defocus disk horizontal radius
    vec3 defocus_disk_v; // Defocus disk vertical radius

    void initialize() {
        // Output Image Settings
        image_height = std::max(int32_t(image_width / aspect_ratio), 1);
        
        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = lookfrom;

        // Viewport Settings
        // double focal_length = (lookfrom - lookat).length();
        double theta = degrees_to_radians(vfov);
        double h = std::tan(theta/2);
        double viewport_height = 2 * h * focus_dist;
        double viewport_width = viewport_height * (double(image_width) / image_height);

        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // vector related to viewport 
        // --------------------------
        /** vector across the vertical viewport_edge */
        const vec3 viewport_u = viewport_width * u;
        /** vector across the horizontal viewport_edge */
        const vec3 viewport_v = viewport_height * -v;
        
        /** vector across the vertical edge of pixel in viewport */
        pixel_delta_u = viewport_u / image_width;
        /** vector across the horizontal edge of pixel in viewport */
        pixel_delta_v = viewport_v / image_height;

        const vec3 viewport_center = center - focus_dist * w;
        const vec3 viewport_upper_left = viewport_center - (viewport_u + viewport_v)/2;
        pixel00_loc = viewport_upper_left + (pixel_delta_u + pixel_delta_v)/2;

        double defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
        // --------------------------
    }
    
    ray get_ray(int32_t i, int32_t j) const {
        const vec3 offset = sample_square();
        const vec3 pixel_sample = pixel00_loc
            + ((i + offset.x()) * pixel_delta_u)
            + ((j + offset.y()) * pixel_delta_v);
        
        const vec3 ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        const vec3 ray_direction = pixel_sample - ray_origin;
        const double ray_time = random_double();

        return ray{ray_origin, ray_direction, ray_time};
    }

    vec3 sample_square() const {
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }
    
    // カメラのdefocus disk内に含まれる点を参照する。
    point3 defocus_disk_sample() const {
        vec3 p = random_in_unit_disk();
        // 行列演算に直せば(defocus_disk_u defocus_disk_v)pと等価
        return center + p[0] * defocus_disk_u + p[1] * defocus_disk_v;
    }

    /**
     * @brief `world`に向けて飛ばした飛ばした光線`r`が何色かを評価する。
     */
    color ray_color(
        const ray& r,
        const hittable& world,
        const int32_t depth
    ) const {
        if (depth <= 0) { return color{0, 0, 0}; }
        
        // Hittableに衝突したときの、その位置に関する情報
        hit_record rec;
        if (not world.hit(r, interval{0.001, infinity}, rec)) {
            return background;
        }
        
        // 物体に衝突した場合には
        // その衝突点からさらにランダムな方向にレイを飛ばし、その飛ばしたレイの色を用いて評価する
        // 光線の逆進性を用いてこれを解釈すると、ある方向から飛んできたレイが反射率の影響を受けながらランダムな方向に飛んでいく過程だとみなせる。
        ray scattered;
        color attenuation;
        color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);
        if (not rec.mat->scatter(r, rec, attenuation, scattered)) { return color_from_emission; }
        color color_from_scatter = attenuation * ray_color(scattered, world, depth - 1);
        return color_from_scatter + color_from_emission;
    }

};

#endif