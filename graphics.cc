#include "graphics.h"

#include <cairommconfig.h>
#include <cairomm/context.h>
#include <cairomm/surface.h>


namespace graphics2 {


namespace detail {


    struct surface_t
    {
        Cairo::RefPtr<Cairo::Surface> surface;
        Cairo::Surface* operator->() { return surface.operator->(); }
    };


    struct context_t
    {
        explicit context_t(const surface_t& surface)
            : context(Cairo::Context::create(surface.surface))
        {}

        Cairo::RefPtr<Cairo::Context> context;
        Cairo::Context* operator->() { return context.operator->(); }
    };


}


surface_t::~surface_t()
{}


void surface_t::show_page()
{
    (*_surface)->show_page();
}


void surface_t::fill(const color_t& color)
{
    detail::context_t context(*_surface);
    context->set_source_rgba(color.red(), color.green(), color.blue(), color.alpha());
    context->paint();
}


void surface_t::fill(const color_t& color, const path_base_t& path)
{
    detail::context_t context(*_surface);
    path.apply_to_context(context);
    context->set_source_rgba(color.red(), color.green(), color.blue(), color.alpha());
    context->fill();
}


void surface_t::stroke(const pen_t& pen, const path_base_t& path)
{
    detail::context_t context(*_surface);
    const auto& color = pen.color();
    context->set_source_rgba(color.red(), color.green(), color.blue(), color.alpha());
    context->set_line_width(pen.width());
    path.apply_to_context(context);
    context->stroke();
}


/*
void surface_t::print(const font_t& font, const pos_t& pos, const std::string& text)
{
    detail::context_t context(*_surface);
    context->move_t(pos.x(), pos.y());
    const auto& color = font.color();
    context->set_source_rgba(color().red(), color().green(), color().blue(), color().alpha());
    font.set_to_context(context);
    context->show_text(text);
}
*/


surface_t::surface_t(detail::surface_t surface)
    : _surface(new detail::surface_t(std::move(surface)))
{
}


image_surface_t::image_surface_t(Format format, double width, double height)
    : surface_t(detail::surface_t{Cairo::ImageSurface::create(format, width, height)})
{
}


void image_surface_t::write_to_png(const std::string& filename)
{
    dynamic_cast<Cairo::ImageSurface&>(*_surface->surface.operator->()).write_to_png(filename);
}


svg_surface_t::svg_surface_t(const std::string& filename, double width, double height)
    : surface_t(detail::surface_t{Cairo::SvgSurface::create(filename, width, height)})
{
}


void line_t::apply_to_context(detail::context_t &context) const
{
    context->move_to(_start.x(), _start.y());
    context->line_to(_end.x(), _end.y());
}


void rectangle_t::apply_to_context(detail::context_t& context) const
{
    context->rectangle(_corner1.x(), _corner1.y(), _corner2.x(), _corner2.y());
}


void arc_t::apply_to_context(detail::context_t& context) const
{
    context->arc(_center.x(), _center.y(), _radius, _angle_start, _angle_stop);
}


}
