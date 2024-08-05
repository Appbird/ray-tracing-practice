#include "vec3.hpp"
#include "color.hpp"

#include <iostream>

int main() {
    double aspect_ratio = 16.0 / 9.0;
    int32_t image_width = 400;
    int32_t image_height = std::max(int32_t(image_width / aspect_ratio), 1);

    double viewport_height = 2.0;
    double viewport_width = viewport_height * (double(image_height) / image_width);

    std::cout << "P3" << std::endl;
    std::cout << image_width << " " << image_height << std::endl;
    std::cout << 255 << std::endl;

    for (int32_t j = 0; j < image_width; j++) {
        std::clog << "\rScanlines remaining: " << (image_height - j) << " " << std::flush;
        for (int32_t i = 0; i < image_height; i++) {
            color pixel_color = color{double(i) / (image_width - 1), double(j) / (image_width - 1), 0};
            write_color(std::cout, pixel_color);
        }
    }
    std::clog << "\rDone.                       \n" << std::flush;
}