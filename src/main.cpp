#include "vec3.hpp"
#include "color.hpp"
#include "ray.hpp"

#include <iostream>

double sq(double x) { return x * x; }

double hit_sphere(
    const point3& center,
    double radius,
    const ray& r
) {
    const vec3 oc = center - r.origin();
    const double a = dot(r.direction(), r.direction());
    const double b = -2 * dot(r.direction(), oc);
    const double c = dot(oc, oc) - sq(radius);
    const double discriminant = sq(b) - 4*a*c;
    
    if (discriminant < 0) { return -1.0; }
    return (-b - std::sqrt(discriminant)) / (2 * a);
    
}

color ray_color(const ray& r) {
    const double t = hit_sphere(vec3{0, 0, -1}, 0.5, r);    
    if (t > 0) {
        const vec3 normal = unit_vector(r.at(t) - vec3{0, 0, -1});
        return 0.5 * (normal + vec3{1, 1, 1});
    }
    vec3 unit_direction = unit_vector(r.direction());
    const double a = 0.5 * (unit_direction.y() + 1.0);
    return (1 - a)*color{1.0, 1.0, 1.0} + a*color{0.5, 0.7, 1.0};
}

int main() {
    double aspect_ratio = 16.0 / 9.0;
    int32_t image_width = 400;
    int32_t image_height = std::max(int32_t(image_width / aspect_ratio), 1);

    double focal_length = 1.0;
    double viewport_height = 2.0;
    double viewport_width = viewport_height * (double(image_width) / image_height);
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
    vec3 viewport_upper_left = viewport_center - (viewport_u + viewport_v)/2;
    vec3 pixel00_loc = viewport_upper_left + (pixel_delta_u + pixel_delta_v)/2;

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