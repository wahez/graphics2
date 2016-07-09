#include "graphics.h"
#include <cmath>
#include <iostream>


int main()
{
    using namespace graphics2;
    auto filename = "image.svg";
    auto width = 600;
    auto height = 400;
    svg_surface_t surface(filename, width, height);

    surface.fill(color_t(0.86, 0.85, 0.47));
    auto rectangle = rectangle_t(0, 0, width, height);
    auto rect_pen = pen_t(20);
    surface.stroke(rect_pen, rectangle);

    auto black = color_t(0, 0, 0, 0.7);
    auto circle = arc_t(
        width / 2.0,
        height / 2.0,
        height / 4.0,
        0.0,
        2.0 * M_PI);
    auto circ_pen = pen_t(black, 20);
    surface.stroke(circ_pen, circle);

    auto line = line_t(
        width / 4.0,
        height / 4.0,
        width * 3.0 / 4.0,
        height * 3.0 / 4.0);
    surface.stroke(circ_pen, line);

    surface.show_page();

    std::cout << "Wrote SVG file \"" << filename << "\"" << std::endl;
    return 0;
}
