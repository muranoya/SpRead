#include <algorithm>
#include <memory>
#include "BasicImage.hpp"

BasicImage::BasicImage(int w, int h, int d)
    : raw_data(nullptr)
    , align_ptr(nullptr)
    , w(w)
    , h(h)
    , d(d)
{
    alloc4ByteAligned(w*h*d, raw_data, align_ptr);
}

BasicImage::BasicImage(int w, int h, int d,
        const unsigned char *data)
    : raw_data(nullptr)
    , align_ptr(nullptr)
    , w(w)
    , h(h)
    , d(d)
{
    alloc4ByteAligned(w*h*d, raw_data, align_ptr);
    std::copy(data, data+w*h*d, align_ptr);
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

    const unsigned char *ptr =
        reinterpret_cast<const unsigned char*>(img.data()[0]);
    /*
    raw_data = new unsigned char[w*h*d];
    */
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
    //raw_data = new unsigned char[w*h*d];
    std::copy(other.raw_data, other.raw_data+w*h*d, align_ptr);
}

BasicImage::BasicImage(BasicImage &&other)
{
    raw_data = other.raw_data;
    other.raw_data = nullptr;
    align_ptr = other.align_ptr;
    other.align_ptr = nullptr;
    w = other.w;
    h = other.h;
    d = other.d;
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

    delete[] raw_data;
    
    alloc4ByteAligned(w*h*d, raw_data, align_ptr);
    //raw_data = new unsigned char[w*h*d];
    std::copy(other.raw_data, other.raw_data+w*h*d, align_ptr);
    
    return *this;
}

BasicImage &
BasicImage::operator=(BasicImage &&other)
{
    delete[] raw_data;
    raw_data = other.raw_data;
    other.raw_data = nullptr;
    align_ptr = other.align_ptr;
    other.align_ptr = nullptr;
    w = other.w;
    h = other.h;
    d = other.d;
    return *this;
}

const unsigned char *
BasicImage::bits() const
{
    return align_ptr;
}

unsigned char *
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
        unsigned char *&rawPtr, unsigned char *&alignPtr)
{
    rawPtr = new unsigned char[size+4];
    
    std::size_t space = size+4;
    void *temp_ptr = rawPtr;
    if (std::align(4, size, temp_ptr, space))
    {
        alignPtr = reinterpret_cast<unsigned char*>(temp_ptr);
    }
    else
    {
        alignPtr = rawPtr;
    }
}

