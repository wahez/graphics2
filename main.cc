#include "graphics.h"
#include <cmath>
#include <iostream>


using namespace graphics2;


const double HEIGHT = 200.0;
const double WIDTH = 400.0;
const double FONT_SIZE = 64.0;
const pos_t text_origin(50, HEIGHT / 2.0 + FONT_SIZE / 2.0);
const double GLYPH_SPACING = 0.1;


struct GlyphBounds
{
    unsigned long glyph;
    double width;
    double height;
};


// an array that stores the bounds of the glyphs that we're going to draw
static const GlyphBounds glyphs[] =
{
    { 'c', 0.45, 0.5 },
    { 'a', 0.45, 0.5 },
    { 'i', 0.2, 0.75 },
    { 'r', 0.4, 0.5 },
    { 'o', 0.44, 0.5 },
    { 'm', 0.75, 0.5 },
    { '!', 0.2, 0.75 }
};


struct font_extents_t
{
    double ascent = 0;
    double descent = 0;
    double height = 0;
    double max_x_advance = 0;
    double max_y_advance = 0;
};


struct text_extents_t
{
    double x_bearing = 0;
    double y_bearing = 0;
    double width = 0;
    double height = 0;
    double x_advance = 0;
    double y_advance = 0;
};


class scaled_font_t {};


class user_font_face_t : public font_face_t
{
public:
    user_font_face_t();

    virtual font_extents_t init(
        const scaled_font_t&,
        const surface_t&) = 0;

    virtual unsigned long unicode_to_glyph(
        const scaled_font_t&,
        unsigned long unicode) = 0;

    virtual text_extents_t render_glyph(
        const scaled_font_t&,
        unsigned long glyph,
        surface_t& surface) = 0;
};


// A *very* simple font that just draws a box for every glyph
class box_font_face_t : public user_font_face_t
{
public:
    font_extents_t init(
        const scaled_font_t&,
        const surface_t&) override
    {
        font_extents_t extents;
        double max = 0;
        for (unsigned int i = 0; i < sizeof (glyphs) / sizeof (GlyphBounds); ++i) {
            if (glyphs[i].width > max)
                max = glyphs[i].width;
        }
        // add some spacing between characters
        max += GLYPH_SPACING;
        extents.max_x_advance = max;
        return extents;
    }

    unsigned long unicode_to_glyph(
        const scaled_font_t&,
        unsigned long unicode) override
    {
        unsigned long glyph = 0;
        // yes this is a stupid an ineffienct way to do this but we only have a few
        // glyphs and this is just demonstration code
        for (unsigned int i = 0; i < sizeof (glyphs) / sizeof (GlyphBounds); ++i) {
            if (glyphs[i].glyph == unicode) {
                // glyph 0 is often a special glyph-not-found value, so offset it by 1
                glyph = i+1;
                break;
            }
        }
        return glyph;
    }

    text_extents_t render_glyph(
        const scaled_font_t&,
        unsigned long glyph,
        surface_t& surface) override
    {
        text_extents_t metrics;
        // check that the glyph is in our table
        if (glyph >= 1 && glyph <= sizeof(glyphs)/sizeof(GlyphBounds)) {
            surface.stroke(
                pen_t(0.05),
                rectangle_t(
                    pos_t(0, 0),
            // Need a negative Y value since the text origin is at the bottom left point
            // and cairo's positive Y axis is down and we want to draw up
                    pos_t(glyphs[glyph-1].width, -glyphs[glyph-1].height)));
            metrics.x_advance = glyphs[glyph-1].width + GLYPH_SPACING;
        }
        return metrics;
    }
};


int main()
{

    image_surface_t surface(Format::FORMAT_ARGB32, WIDTH, HEIGHT);
    surface.fill(color_t(1.0, 1.0, 1.0));
    surface.fill(
        color_t(0.0, 1.0, 0.0, 0.5),
        arc_t(text_origin, FONT_SIZE / 4.0, 0, 2*M_PI));

    surface.print(
        font_t(
            box_font_face_t(),
            color_t(0.8, 0.2, 0.2),
            FONT_SIZE),
        text_origin,
        "graphics2!");

    surface.print(
        font_t(
            toy_font_face_t("Bitstream Charter", FontSlant::FONT_SLANT_NORMAL, FontWeight::FONT_WEIGHT_BOLD),
            color_t(0.2, 0.2, 0.2, 0.3),
            FONT_SIZE),
        text_origin,
        "graphics2!");

    const char* filename = "user-font.png";
    surface.write_to_png(filename);
    std::cout << "Wrote Image " << filename << std::endl;
}
