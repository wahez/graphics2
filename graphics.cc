#include <string>
#include <iostream>
#include <cairommconfig.h>
#include <cairomm/context.h>
#include <cairomm/surface.h>

#include <cmath>


namespace graphics2
{


class surface_t
{
public:
    surface_t(const std::string& filename, double width, double height)
        : _surface(Cairo::SvgSurface::create(filename, width, height))
    {}

    surface_t(Cairo::RefPtr<Cairo::Surface> surface)
        : _surface(std::move(surface))
    {}

    Cairo::Surface* operator->() { return _surface.operator->(); }
    operator Cairo::RefPtr<Cairo::Surface>() { return _surface; }

private:
    Cairo::RefPtr<Cairo::Surface> _surface;
};


class color_t
{
public:
    constexpr color_t(double red, double green, double blue)
        : _red(red)
        , _green(green)
        , _blue(blue)
    {}

    constexpr double red() const { return _red; }
    constexpr double green() const { return _green; }
    constexpr double blue() const { return _blue; }

private:
    double _red;
    double _green;
    double _blue;
};


void fill(surface_t& surface, const color_t& color)
{
    auto cr = Cairo::Context::create(surface);
    cr->set_source_rgb(color.red(), color.green(), color.blue());
    cr->paint();
}


}


int main()
{
    std::string filename = "image.svg";
    double width = 600;
    double height = 400;
    auto surface =
        Cairo::SvgSurface::create(filename, width, height);

    graphics2::surface_t surf(surface);
    graphics2::fill(surf, graphics2::color_t(0.86, 0.85, 0.47));

    auto cr = Cairo::Context::create(surface);

    cr->save();
    // draw a border around the image
    cr->set_line_width(20.0);    // make the line wider
    cr->rectangle(0.0, 0.0, cairo_image_surface_get_width(surface->cobj()), height);
    cr->stroke();

    cr->set_source_rgba(0.0, 0.0, 0.0, 0.7);
    // draw a circle in the center of the image
    cr->arc(width / 2.0, height / 2.0,
            height / 4.0, 0.0, 2.0 * M_PI);
    cr->stroke();

    // draw a diagonal line
    cr->move_to(width / 4.0, height / 4.0);
    cr->line_to(width * 3.0 / 4.0, height * 3.0 / 4.0);
    cr->stroke();
    cr->restore();

    cr->show_page();

    std::cout << "Wrote SVG file \"" << filename << "\"" << std::endl;
    return 0;
}
