#include "graphics.h"
#include <cmath>
#include <iostream>


int main()
{
    using namespace graphics2;

    const auto HEIGHT = 200.0;
    const auto WIDTH = 400.0;
    const auto FONT_SIZE = 64.0;
    const auto text_origin = pos_t(50.0, (HEIGHT / 2.0) + (FONT_SIZE / 2.0));

    image_surface_t surface(Format::FORMAT_ARGB32, WIDTH, HEIGHT);
    surface.fill(color_t(1, 1, 1));
    surface.fill(
        color_t(0, 1, 0, 0.5),
        arc_t(text_origin, FONT_SIZE / 4.0, 0, 2*M_PI));

    surface.print(
        font_t(
            toy_font_face_t(
                "Bitstream Charter",
                FontSlant::FONT_SLANT_ITALIC,
                FontWeight::FONT_WEIGHT_BOLD),
            color_t(0.8, 0.2, 0.2),
            FONT_SIZE),
        text_origin,
        "graphics2!");

    surface.write_to_png("toy-text.png");
}
