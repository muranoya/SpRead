#include <algorithm>
#include <memory>
#include "BasicImage.hpp"

BasicImage::BasicImage(int w, int h, int d, const uchar *data)
    : raw_data(nullptr)
    , align_ptr(nullptr)
    , w(w)
    , h(h)
    , d(d)
{
    alloc4ByteAligned(w*h*d, raw_data, align_ptr);
    if (data) std::copy(data, data+w*h*d, align_ptr);
}

BasicImage::BasicImage(const Fl_Image &img)
    : raw_data(nullptr)
    , align_ptr(nullptr)
    , w(img.w())
    , h(img.h())
    , d(img.d())
{
    if (d <= 0)
    {
        w = h = d = 0;
        return;
    }

    const uchar *ptr =
        reinterpret_cast<const uchar*>(img.data()[0]);
    alloc4ByteAligned(w*h*d, raw_data, align_ptr);
    std::copy(ptr, ptr+w*h*d, align_ptr);
}

BasicImage::BasicImage(const BasicImage &other)
    : raw_data(nullptr)
    , align_ptr(nullptr)
    , w(other.w)
    , h(other.h)
    , d(other.d)
{
    alloc4ByteAligned(w*h*d, raw_data, align_ptr);
    std::copy(other.raw_data, other.raw_data+w*h*d, align_ptr);
}

BasicImage::BasicImage(BasicImage &&other) noexcept
    : raw_data(other.raw_data)
    , align_ptr(other.align_ptr)
    , w(other.w)
    , h(other.h)
    , d(other.d)
{
    if (this == &other) return;

    other.raw_data = nullptr;
    other.align_ptr = nullptr;
}

BasicImage::~BasicImage()
{
    delete[] raw_data;
}

BasicImage &
BasicImage::operator=(const BasicImage &other)
{
    w = other.w;
    h = other.h;
    d = other.d;

    uchar *old_data = raw_data;
    alloc4ByteAligned(w*h*d, raw_data, align_ptr);
    std::copy(other.raw_data, other.raw_data+w*h*d, align_ptr);
    delete[] old_data;
    
    return *this;
}

BasicImage &
BasicImage::operator=(BasicImage &&other) noexcept
{
    if (this == &other) return *this;

    raw_data = other.raw_data;
    other.raw_data = nullptr;
    align_ptr = other.align_ptr;
    other.align_ptr = nullptr;
    w = other.w;
    h = other.h;
    d = other.d;
    return *this;
}

void
BasicImage::convertToMono()
{
    if (d == 1 && !isNull()) return;

    uchar *new_bits, *new_abits;
    alloc4ByteAligned(w*h, new_bits, new_abits);

    const uchar *ap = align_ptr;
    uchar *p = new_abits;
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            int s = 0;
            for (int z = 0; z < d; ++z)
            {
                s += *ap;
            }
            *p = s/d;
            p++;
            ap += d;
        }
    }

    delete raw_data;
    raw_data = new_bits;
    align_ptr = new_abits;
    d = 1;
}

const uchar *
BasicImage::bits() const
{
    return align_ptr;
}

uchar *
BasicImage::bits()
{
    return align_ptr;
}

bool
BasicImage::isNull() const
{
    return w == 0 || h == 0 || d == 0;
}

int
BasicImage::width() const
{
    return w;
}

int
BasicImage::height() const
{
    return h;
}

int
BasicImage::depth() const
{
    return d;
}

void
BasicImage::alloc4ByteAligned(int size,
        uchar *&rawPtr, uchar *&alignPtr)
{
    const int align_val = 4;
    rawPtr = new unsigned char[size+align_val];
    
    std::size_t space = size+align_val;
    void *temp_ptr = rawPtr;
    if (std::align(align_val, size, temp_ptr, space))
    {
        alignPtr = reinterpret_cast<uchar*>(temp_ptr);
    }
    else
    {
        alignPtr = rawPtr;
    }
}

