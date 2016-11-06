#ifndef SIZE_HPP
#define SIZE_HPP

#include <utility>

class Size
{
public:
    explicit Size();
    explicit Size(int w, int h);
    Size(const Size &) = default;
    Size(Size &&) = default;
    ~Size();

    Size &operator=(const Size &) = default;
    Size &operator=(Size &&) = default;
    Size &operator*=(double f);
    Size &operator+=(const Size &size);
    Size &operator-=(const Size &size);
    Size &operator/=(double f);

    Size &w(int nw);
    int  w() const;
    Size &h(int nh);
    int  h() const;

private:
    std::pair<int, int> s;
};

const Size
operator*(double f, const Size &size);

const Size
operator*(const Size &size, double f);

const Size
operator+(const Size &s1, const Size &s2);

const Size
operator-(const Size &s1, const Size &s2);

bool
operator!=(const Size &s1, const Size &s2);

bool
operator==(const Size &s1, const Size &s2);

#endif // SIZE_HPP
