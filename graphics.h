#include <string>
#include <vector>
#include <memory>


namespace graphics2 {


namespace detail {
    struct context_t;
    struct surface_t;
}


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
    virtual void write_to_context(detail::context_t&) const = 0;
};


class surface_t
{
public:
    virtual ~surface_t();
    void show_page();

    void fill(const color_t& color);
    void stroke(const pen_t& pen, const path_base_t& path);

protected:
    explicit surface_t(detail::surface_t surface);
    std::unique_ptr<detail::surface_t> _surface;
};


class svg_surface_t: public surface_t
{
public:
    svg_surface_t(const std::string& filename, double width, double height);
};


class line_t: public path_base_t
{
public:
    line_t(double x1, double y1, double x2, double y2)
        : _x1(x1)
        , _x2(x2)
        , _y1(y1)
        , _y2(y2)
    {}

private:
    void write_to_context(detail::context_t& context) const override;

    double _x1;
    double _x2;
    double _y1;
    double _y2;
};


class rectangle_t: public path_base_t
{
public:
    rectangle_t(double x1, double y1, double x2, double y2)
        : _x1(x1)
        , _x2(x2)
        , _y1(y1)
        , _y2(y2)
    {}

private:
    void write_to_context(detail::context_t& context) const override;

    double _x1;
    double _x2;
    double _y1;
    double _y2;
};


class arc_t: public path_base_t
{
public:
    arc_t(double x, double y, double radius, double angle_start, double angle_stop)
        : _x(x)
        , _y(y)
        , _radius(radius)
        , _angle_start(angle_start)
        , _angle_stop(angle_stop)
    {}

private:
    void write_to_context(detail::context_t& context) const override;

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
