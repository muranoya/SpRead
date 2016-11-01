#include <cmath>
#include <algorithm>
#include "image.hpp"

#include "for_windows_env.hpp"

BasicImage *
nn(const BasicImage &src, double s)
{
    const int w = src.width();
    const int h = src.height();
    const int d = src.depth();
    const int nw = w*s;
    const int nh = h*s;
    const int x1 = w-1;
    const int y1 = h-1;

    BasicImage *img = new BasicImage(nw, nh, d);
    uchar *nbits = img->bits();
    const uchar *bits = src.bits();

    for (int y = 0; y < nh; ++y)
    {
        const int y0 = std::min(static_cast<int>(std::floor(y/s+0.5)), y1)*w*d;
        for (int x = 0; x < nw; ++x)
        {
            for (int i = 0; i < d; ++i)
            {
                nbits[x*d+i] = bits[y0+
                        std::min(static_cast<int>(std::floor(x/s+0.5)), x1)*d+i];
            }
        }
        nbits += nw*d;
    }

    return img;
}

BasicImage *
bl(const BasicImage &src, double s)
{
    const int d = src.depth();
    const int wd = src.width()*d;
    const int nw = src.width()*s;
    const int nh = src.height()*s;
    //const int w1 = src.width()-1;
    //const int h1 = src.height()-1;

    BasicImage *img = new BasicImage(nw, nh, d);
    uchar *nbits = img->bits();
    const uchar *bits = src.bits();

    for (int y = 0; y < nh; ++y)
    {
        const double yg = std::floor(y/s);
        const double ysg = y/s-yg;
        const int yy = yg*wd;
        for (int x = 0; x < nw; ++x)
        {
            const double xg = std::floor(x/s);
            const double xsg = x/s-xg;
            const int xx = xg*d;
            for (int i = 0; i < d; ++i)
            {
                *nbits++ = 
                    (1-xsg) * (1-ysg) * bits[xx + i + yy   ] +
                    (1-xsg) * (ysg)   * bits[xx + i + yy+wd] +
                    (xsg)   * (1-ysg) * bits[xx+d+i + yy   ] +
                    (xsg)   * (ysg)   * bits[xx+d+i + yy+wd];
            }
        }
    }

    return img;
}

static double
bicubic_h(const double t)
{
    const double u = std::fabs(t);
    if (u <= 1.0)
    {
        // u <= 1
        // (a+2)*u^3 - (a+3)*u^2 + 1
        return (u*u*u)-2.0*(u*u)+1.0;
    }
    else if (1.0 < u && u < 2.0)
    {
        // 1 < u < 2
        // a*u^3 - 5a*u^2 + 8a*u - 4a
        return -(u*u*u)+5.0*(u*u)-8.0*u+4.0;
    }
    else
    {
        // 2 < u
        return 0.0;
    }
}

static int
bicubic_matmul(const double d1[4],
        const int d2[4][4], const double d3[4])
{
    double temp[4];
    for (int i = 0; i < 4; ++i)
    {
        temp[i] = d1[0]*d2[0][i]
                + d1[1]*d2[1][i]
                + d1[2]*d2[2][i]
                + d1[3]*d2[3][i];
    }
    return static_cast<int>(
            temp[0]*d3[0] +
            temp[1]*d3[1] +
            temp[2]*d3[2] +
            temp[3]*d3[3]);
}

BasicImage *
bc(const BasicImage &src, double s)
{
    const int w = src.width();
    const int h = src.height();
    const int d = src.depth();
    const int wd = w*d;
    const int w1 = w-1;
    const int w1d = w1*d;
    const int h1 = h-1;
    const int nw = w*s;
    const int nh = h*s;

    BasicImage *img = new BasicImage(nw, nh, d);
    uchar *nbits = img->bits();
    const uchar *bits = src.bits();

    for (int y = 0; y < nh; ++y)
    {
        const int yg = std::floor(y/s);
        const double y2 = y/s-yg;
        const double d1[4] = {
            bicubic_h(1+y2),
            bicubic_h(y2),
            bicubic_h(1-y2),
            bicubic_h(2-y2)};

        for (int x = 0; x < nw; ++x)
        {
            const int xg = std::floor(x/s);
            const double x2 = x/s-xg;
            const double d3[4] = {
                bicubic_h(1+x2),
                bicubic_h(x2),
                bicubic_h(1-x2),
                bicubic_h(2-x2)};

            for (int dd = 0; dd < d; ++dd)
            {
                int d2[4][4];
                for (int i = 0; i < 4; ++i)
                {
                    for (int k = 0; k < 4; ++k)
                    {
                        d2[i][k] = 
                            bits[std::min(std::max((xg+k-1)*d, 0), w1d)+dd
                               + std::min(std::max( yg+i-1,    0),  h1)*wd];
                    }
                }
                *nbits++ = std::min(std::max(bicubic_matmul(d1, d2, d3), 0), 0xFF);
            }
        }
    }

    return img;
}

