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
class numeric_range
{
public:
    using range_type = TYPE;

    constexpr TYPE min() const { return MIN; }
    constexpr TYPE max() const { return MAX; }

    template<typename T>
    constexpr bool is_valid(const T& t) const { return t >= min() && t <= max(); }
};


template<typename TYPE, typename CHANNELRANGE>
class channel_t: public CHANNELRANGE
{
public:
    using type = TYPE;
    using range = CHANNELRANGE;

    explicit constexpr channel_t(type value)
        : _value(std::move(value))
    {
    }

    constexpr type value() const { return _value; }
    constexpr void value(type val) { _value = std::move(val); }

    template<typename...ARGS>
    explicit constexpr channel_t(const channel_t<ARGS...>& ochan)
        : _value(unscaled(ochan.scaled()))
    {}

    constexpr bool is_valid() const { return range::is_valid(value()); }

private:
    template<typename, typename> friend class channel_t;
    constexpr double scaled() const { return (1.0*value() - range::min()) / (range::max() - range::min()); }
    constexpr TYPE unscaled(double d) const { return d * (range::max() - range::min()) + range::min(); }

    type _value;
};


template<typename... Args>
std::ostream& operator<<(std::ostream& os, const channel_t<Args...>& channel)
{
    os << channel.value();
    return os;
}


using channel = channel_t<double, numeric_range<std::int8_t, 0, 1>>;

template<int BITS>
using channel_bits = channel_t<uint_at_least_t<BITS>, numeric_range<uint_at_least_t<BITS>, 0, (1<<BITS)-1>>;


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


template<typename TYPE>
class position_t
{
public:
    TYPE x;
    TYPE y;
};


/*
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
    constexpr graphics2::channel_t<int, graphics2::numeric_range<int, 0, 10>> intColorChannel{5};
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
