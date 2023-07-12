#pragma once
#include <cstdint>
#include <array>

namespace embedded
{

template<typename T = int>
struct Size
{
    T width;
    T height;

    constexpr Size operator+(Size other) const
    {
        return { width + other.width, height + other.height };
    }

    constexpr Size operator-(Size other) const
    {
        return { width - other.width, height - other.height };
    }

    constexpr Size operator*(T scale) const
    {
        return { width * scale, height * scale };
    }

    constexpr Size operator/(T scale) const
    {
        return { width / scale, height / scale };
    }
};

template<typename T = int>
struct Point
{
    T x;
    T y;

    constexpr Point operator+(const Size<T> size) const
    {
        return Point { x + size.width, y + size.height };
    }

    Point &operator+=(const Size<T> &size)
    {
        x += size.width;
        y += size.height;
        return *this;
    }

    bool operator==(const Point other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Point other) const
    {
        return x != other.x || y != other.y;
    }
};

template<typename T = int>
struct Rect
{
    Point<T> topLeft;
    Size<T> size;

    constexpr Rect operator+(const Size<T> s) const
    {
        return Rect { topLeft + s, size };
    }

    Rect &operator+=(const Size<T> s)
    {
        topLeft += s;
        return *this;
    }

    Point<T> bottomLeft() const
    {
        return { topLeft.x, topLeft.y + size.height - 1 };
    }

    Point<T> topRight() const
    {
        return { topLeft.x + size.width - 1, topLeft.y };
    }

    Point<T> bottomRight() const
    {
        return topLeft + (size - Size<T> { 1, 1 });
    }
};

template<typename T = int>
class Matrix2x2
{
public:
    Matrix2x2(T a, T b, T c, T d) : data { a, b, c, d } {}

    Point<T> operator*(Point<T> v)
    {
        return { data[0] * v.x + data[1] * v.y, data[2] * v.x + data[3] * v.y };
    }

private:
    std::array<T, 4> data;
};

}
