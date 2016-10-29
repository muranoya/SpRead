#include "Point.hpp"

using namespace std;

Point::Point()
    : p(make_pair(0, 0))
{
}

Point::Point(int x, int y)
    : p(make_pair(x, y))
{
}

Point::~Point()
{
}

Point &
Point::operator*=(double f)
{
    return x(x() * f).y(y() * f);
}


Point &
Point::operator+=(const Point &size)
{
    return x(x() + size.x()).y(y() + size.y());
}

Point &
Point::operator-=(const Point &size)
{
    return x(x() - size.x()).y(y() - size.y());
}

Point &
Point::operator/=(double f)
{
    return x(x() / f).y(y() / f);
}

Point &
Point::x(int nx)
{
    p.first = nx;
    return *this;
}

int
Point::x() const
{
    return p.first;
}

Point &
Point::y(int ny)
{
    p.second = ny;
    return *this;
}

int
Point::y() const
{
    return p.second;
}

const Point
operator*(double f, const Point &size)
{
    return Point(size.x() * f, size.y() * f);
}

const Point
operator*(const Point &size, double f)
{
    return Point(size.x() * f, size.y() * f);
}

const Point
operator+(const Point &s1, const Point &s2)
{
    return Point(s1.x() + s2.x(), s1.y() + s2.y());
}

const Point
operator-(const Point &s1, const Point &s2)
{
    return Point(s1.x() - s2.x(), s1.y() - s2.y());
}

bool
operator!=(const Point &s1, const Point &s2)
{
    return s1.x() != s2.x() || s1.y() != s2.y();
}

bool
operator==(const Point &s1, const Point &s2)
{
    return !(s1 != s2);
}
