#ifndef POINT_HPP
#define POINT_HPP

#include <utility>

class Point
{
public:
    explicit Point();
    explicit Point(int x, int y);
    Point(const Point &) = default;
    Point(Point &&) = default;
    ~Point();

    Point &operator=(const Point &) = default;
    Point &operator=(Point &&) = default;
    Point &operator*=(double f);
    Point &operator+=(const Point &size);
    Point &operator-=(const Point &size);
    Point &operator/=(double f);

    Point &x(int nx);
    int  x() const;
    Point &y(int ny);
    int  y() const;

private:
    std::pair<int, int> p;
};

const Point
operator*(double f, const Point &size);

const Point
operator*(const Point &size, double f);

const Point
operator+(const Point &s1, const Point &s2);

const Point
operator-(const Point &s1, const Point &s2);

bool
operator!=(const Point &s1, const Point &s2);

bool
operator==(const Point &s1, const Point &s2);

#endif // POINT_HPP
