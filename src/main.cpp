#include <iostream>

int main() {
    int32_t image_width = 256;
    int32_t image_height = 256;

    std::cout << "P3" << std::endl;
    std::cout << image_width << " " << image_height << std::endl;
    std::cout << 255 << std::endl;

    for (int32_t i = 0; i < image_width; i++) {
        std::clog << "\rScanlines remaining: " << (image_height - i) << " " << std::flush;
        for (int32_t j = 0; j < image_height; j++) {
            auto r = double(i) / (image_width - 1);
            auto g = double(j) / (image_width - 1);
            auto b = 0.0;

            int32_t ir = int32_t(255.999 * r);
            int32_t ig = int32_t(255.999 * g);
            int32_t ib = int32_t(255.999 * b);

            std::cout << ir << " " << ig << " " << ib << std::endl;
        }
    }
    std::clog << "\rDone.                       \n" << std::flush;
}