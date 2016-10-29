#include "Size.hpp"

using namespace std;

Size::Size()
    : s(make_pair(0, 0))
{
}

Size::Size(int w, int h)
    : s(make_pair(w, h))
{
}

Size::~Size()
{
}

Size &
Size::operator*=(double f)
{
    return w(w() * f).h(h() * f);
}


Size &
Size::operator+=(const Size &size)
{
    return w(w() + size.w()).h(h() + size.h());
}

Size &
Size::operator-=(const Size &size)
{
    return w(w() - size.w()).h(h() - size.h());
}

Size &
Size::operator/=(double f)
{
    return w(w() / f).h(h() / f);
}

Size &
Size::w(int nw)
{
    s.first = nw;
    return *this;
}

int
Size::w() const
{
    return s.first;
}

Size &
Size::h(int nh)
{
    s.second = nh;
    return *this;
}

int
Size::h() const
{
    return s.second;
}

const Size
operator*(double f, const Size &size)
{
    return Size(size.w() * f, size.h() * f);
}

const Size
operator*(const Size &size, double f)
{
    return Size(size.w() * f, size.h() * f);
}

const Size
operator+(const Size &s1, const Size &s2)
{
    return Size(s1.w() + s2.w(), s1.h() + s2.h());
}

const Size
operator-(const Size &s1, const Size &s2)
{
    return Size(s1.w() - s2.w(), s1.h() - s2.h());
}

bool
operator!=(const Size &s1, const Size &s2)
{
    return s1.w() != s2.w() || s1.h() != s2.h();
}

bool
operator==(const Size &s1, const Size &s2)
{
    return !(s1 != s2);
}
