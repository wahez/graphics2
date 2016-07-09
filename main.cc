#include "graphics.h"


int main()
{
    using namespace graphics2;
    svg_surface_t surf("image.svg", 600, 400);

    surf.fill(color_t(0.86, 0.85, 0.47));
    auto rectangle = rectangle_t(0, 0, surf.width(), surf.height());
    pen_t rect_pen(20);
    surf.stroke(rect_pen, rectangle);

    auto black = color_t(0, 0, 0, 0.7);
    auto circle = arc_t(
        width / 2.0,
        height / 2.0,
        height / 4.0,
        0.0,
        2.0 * M_PI);
    pen_t circ_pen(black, 20);
    surf.stroke(circ_pen, circle);

    auto line = line_t(
        width / 4.0,
        height / 4.0,
        width * 3.0 / 4.0,
        height * 3.0 / 4.0);
    surf.stroke(circ_pen, line);

    surf.show_page();

    std::cout << "Wrote SVG file \"" << filename << "\"" << std::endl;
    return 0;
}
