#ifndef COLOR_H
#define COLOR_H

#include "interval.hpp"
#include "vec3.hpp"

using color = vec3;

inline double linear_to_gamma(double x) {
    if (x > 0) { return std::sqrt(x); }
    return 0;
}

void write_color(std::ostream& out, const color& pixel_color) {
    double r = pixel_color.x();
    double g = pixel_color.y();
    double b = pixel_color.z();
    
    // ここで受け渡されたrgbは物理的なエネルギーの強度を表しているが、
    // 実際に出力すべき値は人間が感じる明るさの強度である。
    // ガンマ空間に変換することで、エネルギーの強度から人間が感じる明るさの強度に変換する。
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    // Translate the [0,1] component values to the byte range [0,255].
    static const interval intensity{0, (1 - 1e-3)};
    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));

    // Write out the pixel color components.
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif