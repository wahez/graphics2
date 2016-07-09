#include <string>
#include <vector>
#include <memory>

#include <string>
#include <iostream>
#include <cairommconfig.h>
#include <cairomm/context.h>
#include <cairomm/surface.h>

#include <cmath>


namespace graphics2
{


class color_t
{
public:
    constexpr color_t(double red, double green, double blue, double alpha=1)
        : _red(red)
        , _green(green)
        , _blue(blue)
        , _alpha(alpha)
    {}

    constexpr double red() const { return _red; }
    constexpr double green() const { return _green; }
    constexpr double blue() const { return _blue; }
    constexpr double alpha() const { return _alpha; }

private:
    double _red;
    double _green;
    double _blue;
    double _alpha;
};


class pen_t
{
public:
    explicit constexpr pen_t(double width)
        : _color(0, 0, 0)
        , _width(width)
    {}
    explicit constexpr pen_t(color_t color, double width=2)
        : _color(std::move(color))
        , _width(width)
    {}

    constexpr double width() const { return _width; }
    const color_t& color() const { return _color; }

private:
    color_t _color;
    double _width = 2;
};


class path_base_t
{
public:
    virtual ~path_base_t() {}

private:
    friend class path_t;
    friend class surface_t;
    virtual void write_to_context(Cairo::Context&) const = 0;
};


class surface_t
{
public:
    virtual ~surface_t() {}
    explicit surface_t(Cairo::RefPtr<Cairo::Surface> surface)
        : _surface(std::move(surface))
    {}

    double width() const { return cairo_image_surface_get_width(_surface->cobj()); }
    double height() const { return cairo_image_surface_get_height(_surface->cobj()); }
    void show_page() { _surface->show_page(); }

    void fill(const color_t& color)
    {
        auto cr = Cairo::Context::create(_surface);
        cr->set_source_rgba(color.red(), color.green(), color.blue(), color.alpha());
        cr->paint();
    }

    void stroke(const pen_t& pen, const path_base_t& path)
    {
        auto cr = Cairo::Context::create(_surface);
        const auto& color = pen.color();
        cr->set_source_rgba(color.red(), color.green(), color.blue(), color.alpha());
        cr->set_line_width(pen.width());
        path.write_to_context(*(cr.operator->()));
        cr->stroke();
    }

private:
    Cairo::RefPtr<Cairo::Surface> _surface;
};


class svg_surface_t: public surface_t
{
public:
    svg_surface_t(const std::string& filename, double width, double height)
        : surface_t(Cairo::SvgSurface::create(filename, width, height))
    {}
};


class line: public path_base_t
{
public:
    line(double x1, double y1, double x2, double y2)
        : _x1(x1)
        , _x2(x2)
        , _y1(y1)
        , _y2(y2)
    {}

private:
    void write_to_context(Cairo::Context& context) const override
    {
        context.move_to(_x1, _y1);
        context.line_to(_x2, _y2);
    }

    double _x1;
    double _x2;
    double _y1;
    double _y2;
};


class rectangle: public path_base_t
{
public:
    rectangle(double x1, double y1, double x2, double y2)
        : _x1(x1)
        , _x2(x2)
        , _y1(y1)
        , _y2(y2)
    {}

private:
    void write_to_context(Cairo::Context& context) const override
    {
        context.rectangle(_x1, _y1, _x2, _y2);
    }

    double _x1;
    double _x2;
    double _y1;
    double _y2;
};


class arc: public path_base_t
{
public:
    arc(double x, double y, double radius, double angle_start, double angle_stop)
        : _x(x)
        , _y(y)
        , _radius(radius)
        , _angle_start(angle_start)
        , _angle_stop(angle_stop)
    {}

private:
    void write_to_context(Cairo::Context& context) const override
    {
        context.arc(_x, _y, _radius, _angle_start, _angle_stop);
    }

    double _x;
    double _y;
    double _radius;
    double _angle_start;
    double _angle_stop;
};


class path_t : public path_base_t
{
public:
    template<typename Path>
    explicit path_t(Path path)
    {
        _parts.emplace_back(std::make_unique<Path>(std::forward<Path>(path)));
    }

    bool is_empty() const { return _parts.empty(); }

private:
    // deprecated
    void write_to_context(Cairo::Context& context) const
    {
        for (const auto& part: _parts)
        {
            part->write_to_context(context);
        }
    }

    std::vector<std::unique_ptr<path_base_t>> _parts;
};


}


int main()
{
    std::string filename = "image.svg";
    double width = 600;
    double height = 400;
    auto surface =
        Cairo::SvgSurface::create(filename, width, height);

    graphics2::surface_t surf(surface);
    surf.fill(graphics2::color_t(0.86, 0.85, 0.47));
    graphics2::rectangle rectangle(0, 0, surf.width(), surf.height());
    graphics2::pen_t rect_pen(20);
    surf.stroke(rect_pen, rectangle);

    auto black = graphics2::color_t(0, 0, 0, 0.7);
    auto circle = graphics2::arc(
        width / 2.0,
        height / 2.0,
        height / 4.0,
        0.0,
        2.0 * M_PI);
    graphics2::pen_t circ_pen(black, 20);
    surf.stroke(circ_pen, circle);

    graphics2::line line(
        width / 4.0,
        height / 4.0,
        width * 3.0 / 4.0,
        height * 3.0 / 4.0);
    surf.stroke(circ_pen, line);

    surf.show_page();

    std::cout << "Wrote SVG file \"" << filename << "\"" << std::endl;
    return 0;
}
