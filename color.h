//#include <cairomm/context.h>
#include <utility>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <iostream>


namespace graphics2 {


template<int BITS> struct uint_at_least;
template<> struct uint_at_least< 0> { using type = void; };
template<> struct uint_at_least< 8> { using type = std::uint8_t; };
template<> struct uint_at_least<16> { using type = std::uint16_t; };
template<> struct uint_at_least<32> { using type = std::uint32_t; };
template<> struct uint_at_least<64> { using type = std::uint64_t; };
template<int BITS> struct uint_at_least { using type = typename uint_at_least<BITS+1>::type; };

template<int BITS> using uint_at_least_t = typename uint_at_least<BITS>::type;


template<typename TYPE, TYPE MIN, TYPE MAX>
class numeric_range_t
{
public:
    using range_type = TYPE;

    constexpr range_type min() const { return MIN; }
    constexpr range_type max() const { return MAX; }

    template<typename T>
    constexpr bool is_valid(const T& t) const { return t >= min() && t <= max(); }
};


template<typename TYPE>
class numeric_range
{
public:
    using range_type = TYPE;

    numeric_range(range_type min, range_type max)
        : _min(std::move(min))
        , _max(std::move(max))
    {}

    constexpr range_type min() const { return _min; }
    constexpr range_type max() const { return _max; }

    template<typename T>
    constexpr bool is_valid(const T& t) const { return t >= min() && t <= max(); }

private:
    range_type _min;
    range_type _max;
};


template<typename TYPE, typename RANGE>
class value_in_range: public RANGE
{
public:
    using type = TYPE;
    using range = RANGE;

    explicit constexpr value_in_range(type value)
        : _value(std::move(value))
    {
    }

    constexpr type value() const { return _value; }
    constexpr void value(type val) { _value = std::move(val); }

    template<typename...ARGS>
    explicit constexpr value_in_range(const value_in_range<ARGS...>& ovalue)
        : _value(unscaled(ovalue.scaled()))
    {}

    constexpr bool is_valid() const { return range::is_valid(value()); }

private:
    template<typename, typename> friend class value_in_range;
    // scales to double between 0 and 1
    constexpr double scaled() const { return (1.0*value() - range::min()) / (range::max() - range::min()); }
    constexpr TYPE unscaled(double d) const { return d * (range::max() - range::min()) + range::min(); }

    type _value;
};


template<typename... Args>
std::ostream& operator<<(std::ostream& os, const value_in_range<Args...>& value_in_range)
{
    os << value_in_range.value();
    return os;
}


using channel = value_in_range<double, numeric_range_t<std::int8_t, 0, 1>>;

template<int BITS>
using channel_bits = value_in_range<uint_at_least_t<BITS>, numeric_range_t<uint_at_least_t<BITS>, 0, (1<<BITS)-1>>;


template<typename RCHANNEL, typename GCHANNEL, typename BCHANNEL, typename ALPHACHANNEL>
class color_t
{
public:
    using red_channel = RCHANNEL;
    using green_channel = GCHANNEL;
    using blue_channel = BCHANNEL;
    using alpha_channel = ALPHACHANNEL;

    explicit constexpr color_t(
            typename red_channel::type red,
            typename green_channel::type green,
            typename blue_channel::type blue,
            typename alpha_channel::type alpha)
        : _red(std::move(red))
        , _green(std::move(green))
        , _blue(std::move(blue))
        , _alpha(std::move(alpha))
    {}

    explicit constexpr color_t(
            red_channel red,
            green_channel green,
            blue_channel blue,
            alpha_channel alpha)
        : _red(std::move(red))
        , _green(std::move(green))
        , _blue(std::move(blue))
        , _alpha(std::move(alpha))
    {}

    constexpr red_channel red() const { return _red; }
    constexpr green_channel green() const { return _green; }
    constexpr blue_channel blue() const { return _blue; }
    constexpr alpha_channel alpha() const { return _alpha; }
    constexpr void red(red_channel r) { _red = std::move(r); }
    constexpr void green(green_channel g) { _green = std::move(g); }
    constexpr void blue(blue_channel b) { _blue = std::move(b); }
    constexpr void alpha(alpha_channel a) { _alpha = std::move(a); }

    constexpr bool is_valid() const
    {
        return
            _red.is_valid() &&
            _green.is_valid() &&
            _blue.is_valid() &&
            _alpha.is_valid();
    }

    template<typename... Args>
    explicit constexpr color_t(const color_t<Args...>& color)
        : _red(color.red())
        , _green(color.green())
        , _blue(color.blue())
        , _alpha(color.alpha())
    {}

private:
    red_channel _red;
    green_channel _green;
    blue_channel _blue;
    alpha_channel _alpha;
};


template<typename... Args>
std::ostream& operator<<(std::ostream& os, const color_t<Args...>& color)
{
    os << '(' << color.red()
       << ',' << color.green()
       << ',' << color.blue()
       << ',' << color.alpha()
       << ')';
    return os;
}


using color = color_t<channel, channel, channel, channel>;

template<int RB, int GB, int BB, int AB>
using color_bits = color_t<channel_bits<RB>, channel_bits<GB>, channel_bits<BB>, channel_bits<AB>>;


/*
template<typename XTYPE, typename YTYPE>
class position_t
{
public:
    using x_range_type = XTYPE;
    using y_range_type = YTYPE;
    using typename x_type = x_range_type::type;
    using typename y_type = y_range_type::type;

    constexpr x_type x() const { return _x.value(); }
    constexpr x_range_type x_range() const { return _x; }
    constexpr y_type y() const { return _y.value(); }
    constexpr y_range_type y_range() const { return _y; }
    constexpr void x() { _x.value(); }
    constexpr void x_range() { _x; }
    constexpr void y() { _y.value(); }
    constexpr void y_range() { _y; }


public:
    friend class surface;
    constexpr position_t(x_type x, y_type y)
        : _x(std::move(x))
        , _y(std::move(y))
    {}

    x_range_type _x;
    y_range_type _y;
};


class surface
{
    constexpr position create_position(x_type x, y_type y) const;
};




template<typename COLOR>
class surface_t
{
public:

};


template<typename COLOR>
class image_surface_t
{
public:
    using color = COLOR;

    image_surface_t(size_t width, size_t height);

    void stroke(const Pen&, const Path&);
    void fill(const Pattern&, const Path&);
};


using surface = surface_t<color>;


class Pen
{
    StrokePattern strokePattern;
    Color color;
    double width;
};


class FillPattern;


class Path
{
public:
    Path operator+=(const Path&);

private:
    std::vector<Position> _points;
};

class Rectangle : public Path { };


class ClosedPath;


bool Paint(Surface, Pen, Path);


bool Paint(Surface, FillPattern, ClosedPath);
*/


}


void test_at_least()
{
    static_assert(std::is_same<void, graphics2::uint_at_least_t<0>>::value, "Zero bit unsigned int should be void");
    static_assert(std::is_same<std::uint8_t, graphics2::uint_at_least_t<1>>::value, "Zero bit unsigned int should be void");
    static_assert(std::is_same<std::uint8_t, graphics2::uint_at_least_t<8>>::value, "Zero bit unsigned int should be void");
    static_assert(std::is_same<std::uint16_t, graphics2::uint_at_least_t<9>>::value, "Zero bit unsigned int should be void");
    static_assert(std::is_same<std::uint16_t, graphics2::uint_at_least_t<16>>::value, "Zero bit unsigned int should be void");
    static_assert(std::is_same<std::uint32_t, graphics2::uint_at_least_t<17>>::value, "Zero bit unsigned int should be void");
    static_assert(std::is_same<std::uint32_t, graphics2::uint_at_least_t<32>>::value, "Zero bit unsigned int should be void");
    static_assert(std::is_same<std::uint64_t, graphics2::uint_at_least_t<33>>::value, "Zero bit unsigned int should be void");
    static_assert(std::is_same<std::uint64_t, graphics2::uint_at_least_t<64>>::value, "Zero bit unsigned int should be void");
//    static_assert(std::is_same<std::uint8_t, graphics2::uint_at_least_t<65>>::value, "Zero bit unsigned int should be void");
}


template<typename TYPE, TYPE r>
int p() { return r; }

void test_channel()
{
    constexpr graphics2::channel colorChannel{0.5};
    constexpr graphics2::value_in_range<int, graphics2::numeric_range_t<int, 0, 10>> intColorChannel{5};
    p<int, intColorChannel.value()>();
}

int test_channel_bits()
{
    constexpr graphics2::channel_bits<5> colorChannel{100};
    return p<decltype(colorChannel)::type, colorChannel.value()>();
}

int test_color()
{
    constexpr graphics2::color c(0, 0.5, 1, 0.5);
    static_assert(c.red().value() == 0, "");
    static_assert(c.is_valid(), "c should be valid");
    constexpr graphics2::color_bits<1, 2, 3, 4> c2(c);
    static_assert(c2.red().value() == 0, "red");
    static_assert(c2.green().value() == 1, "green");
    static_assert(c2.blue().value() == 7, "blue");
    static_assert(c2.alpha().value() == 7, "alpha");
}

int main()
{
    test_color();
}
