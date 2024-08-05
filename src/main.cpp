#include "vec3.hpp"
#include "color.hpp"
#include "ray.hpp"

#include <iostream>

color ray_color(const ray& r) {
    vec3 unit_direction = unit_vector(r.direction());
    double t = 0.5 * (unit_direction.y() + 1.0);
    return (1 - t)*color{1.0, 1.0, 1.0} + t*color{0.5, 0.7, 1.0};
}


int main() {
    double aspect_ratio = 16.0 / 9.0;
    int32_t image_width = 400;
    int32_t image_height = std::max(int32_t(image_width / aspect_ratio), 1);

    double focal_length = 1.0;
    double viewport_height = 2.0;
    double viewport_width = viewport_height * (double(image_height) / image_width);
    point3 camera_center = {0, 0, 0};

    /** vector across the vertical viewport_edge */
    vec3 viewport_u = {viewport_width, 0, 0};
    /** vector across the horizontal viewport_edge */
    vec3 viewport_v = {0, -viewport_height, 0};

    /** vector across the vertical edge of pixel in viewport */
    vec3 pixel_delta_u = viewport_u / image_width;
    /** vector across the horizontal edge of pixel in viewport */
    vec3 pixel_delta_v = viewport_v / image_height;

    vec3 viewport_center = camera_center - vec3{0, 0, focal_length};
    vec3 viewport_upper_left = viewport_center - viewport_u/2 - viewport_v/2;
    vec3 pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    std::cout << "P3" << std::endl;
    std::cout << image_width << " " << image_height << std::endl;
    std::cout << 255 << std::endl;

    for (int32_t j = 0; j < image_height; j++) {
        std::clog << "\rScanlines remaining: " << (image_height - j) << " " << std::flush;
        for (int32_t i = 0; i < image_width; i++) {
            vec3 pixel_center = pixel00_loc + i*pixel_delta_u + j*pixel_delta_v;
            vec3 ray_direction = pixel_center - camera_center;
            ray r{camera_center, ray_direction};

            color pixel_color = ray_color(r);
            write_color(std::cout, pixel_color);
        }
    }
    std::clog << "\rDone.                       \n" << std::flush;
}