#include "graphics.h"
#include <cmath>
#include <iostream>


void image()
{
    using namespace graphics2;

    auto width = 600;
    auto height = 400;
    image_surface_t surface(graphics2::Format::FORMAT_ARGB32, width, height);

    surface.fill(color_t(0.86, 0.85, 0.47));

    surface.stroke(pen_t(20), rectangle_t(0, 0, width, height));

    auto pen = pen_t(color_t(0, 0, 0, 0.7), 20);
    surface.stroke(
        pen,
        arc_t(
            width/2,
            height/2,
            height/4,
            0,
            2*M_PI));

    surface.stroke(
        pen,
        line_t(
            width / 4.0,
            height / 4.0,
            width * 3.0 / 4.0,
            height * 3.0 / 4.0));

    std::string filename = "image.png";
    surface.write_to_png(filename);

    std::cout << "Wrote png file \"" << filename << "\"" << std::endl;
}
