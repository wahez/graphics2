#include "graphics.h"

#include <cairommconfig.h>
#include <cairomm/context.h>
#include <cairomm/surface.h>


namespace graphics2
{


class surface_t
{
public:
    virtual ~surface_t() {}
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

protected:
    explicit surface_t(Cairo::RefPtr<Cairo::Surface> surface)
        : _surface(std::move(surface))
    {}

    Cairo::RefPtr<Cairo::Surface> _surface;
};


class svg_surface_t: public surface_t
{
public:
    svg_surface_t(const std::string& filename, double width, double height)
        : surface_t(Cairo::SvgSurface::create(filename, width, height))
    {}
};


}
