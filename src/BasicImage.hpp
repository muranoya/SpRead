#ifndef BASICIMAGE_HPP
#define BASICIMAGE_HPP

#include <FL/Fl_Image.H>

class BasicImage
{
public:
    explicit BasicImage(int w, int h, int d);
    explicit BasicImage(int w, int h, int d,
            const unsigned char *data);
    explicit BasicImage(const Fl_Image &img);
    BasicImage(const BasicImage &other);
    BasicImage(BasicImage &&other);
    ~BasicImage();

    BasicImage &operator=(const BasicImage &other);
    BasicImage &operator=(BasicImage &&other);

    const unsigned char *bits() const;
    unsigned char *bits();

    bool isNull() const;
    int width() const;
    int height() const;
    int depth() const;

private:
    unsigned char *raw_data;
    // 4byte aligned pointer of raw_data.
    // so, you must not delete[] align_ptr;
    unsigned char *align_ptr;
    int w;
    int h;
    int d;

    static void alloc4ByteAligned(int size,
            unsigned char *&rawPtr, unsigned char *&alignPtr);
};

#endif // BASICIMAGE_HPP
