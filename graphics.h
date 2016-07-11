#include <cairomm/enums.h>
#include <string>
#include <vector>
#include <memory>


namespace graphics2 {


namespace detail {
    struct context_t;
    struct surface_t;
}


using Format = Cairo::Format;


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


class pos_t
{
public:
    constexpr pos_t(double x, double y)
        : _x(x)
        , _y(y)
    {}

    constexpr double x() const { return _x; }
    constexpr double y() const { return _y; }
    constexpr void x(double x) { _x = x; }
    constexpr void y(double y) { _y = y; }

private:
    double _x;
    double _y;
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
    virtual void write_to_context(detail::context_t&) const = 0;
};


class font_face_t
{
public:
};


class toy_font_face_t
{
public:
};


class font_t
{
public:
    font_t(font_face_t, color_t, double size);
};


class surface_t
{
public:
    virtual ~surface_t();
    void show_page();

    void fill(const color_t&);
    void fill(const color_t&, const path_base_t&);
    void stroke(const pen_t&, const path_base_t&);
    void print(const font_t&, const pos_t&, const std::string&);

protected:
    explicit surface_t(detail::surface_t);
    std::unique_ptr<detail::surface_t> _surface;
};


class image_surface_t: public surface_t
{
public:
    image_surface_t(Format, double width, double height);
    void write_to_png(const std::string& filename);
};


class svg_surface_t: public surface_t
{
public:
    svg_surface_t(const std::string& filename, double width, double height);
};


class line_t: public path_base_t
{
public:
    line_t(pos_t start, pos_t end)
        : _start(start)
        , _end(end)
    {}

private:
    void write_to_context(detail::context_t&) const override;

    pos_t _start;
    pos_t _end;
};


class rectangle_t: public path_base_t
{
public:
    rectangle_t(pos_t corner1, pos_t corner2)
        : _corner1(corner1)
        , _corner2(corner2)
    {}

private:
    void write_to_context(detail::context_t&) const override;

    pos_t _corner1;
    pos_t _corner2;
};


class arc_t: public path_base_t
{
public:
    arc_t(pos_t center, double radius, double angle_start, double angle_stop)
        : _center(center)
        , _radius(radius)
        , _angle_start(angle_start)
        , _angle_stop(angle_stop)
    {}

private:
    void write_to_context(detail::context_t&) const override;

    pos_t _center;
    double _radius;
    double _angle_start;
    double _angle_stop;
};


class path_t : public path_base_t
{
public:
    path_t() = default;
    path_t(path_t&&) = default;
    path_t& operator=(path_t&&) = default;

    template<typename Path>
    explicit path_t(Path path)
    {
        _parts.emplace_back(std::make_unique<Path>(std::forward<Path>(path)));
    }

    bool is_empty() const { return _parts.empty(); }

    template<typename Path>
    std::enable_if_t<std::is_base_of<path_base_t, std::decay_t<Path>>::value, path_t&> operator+=(Path&& path)
    {
        _parts.emplace_back(new std::decay_t<Path>(std::forward<Path>(path)));
        return *this;
    }

private:
    // deprecated
    void write_to_context(detail::context_t& context) const
    {
        for (const auto& part: _parts)
        {
            part->write_to_context(context);
        }
    }

    std::vector<std::unique_ptr<path_base_t>> _parts;
};


}
