#ifndef BASICIMAGE_HPP
#define BASICIMAGE_HPP

#include <FL/Fl_Image.H>

class BasicImage
{
public:
    explicit BasicImage(int w, int h, int d,
            const uchar *data = nullptr);
    explicit BasicImage(const Fl_Image &img);
    BasicImage(const BasicImage &other);
    BasicImage(BasicImage &&other) noexcept;
    ~BasicImage();

    BasicImage &operator=(const BasicImage &other);
    BasicImage &operator=(BasicImage &&other) noexcept;

    void convertToMono();

    const uchar *bits() const;
    uchar *bits();

    bool isNull() const;
    int width() const;
    int height() const;
    int depth() const;

private:
    uchar *raw_data;
    // 4byte aligned pointer of raw_data.
    // so, you must not delete[] align_ptr;
    uchar *align_ptr;
    int w;
    int h;
    int d;

    static void alloc4ByteAligned(int size,
            uchar *&rawPtr, uchar *&alignPtr);
};

#endif // BASICIMAGE_HPP
