#ifndef PDFFILE_HPP
#define PDFFILE_HPP

#include "ImageFile.hpp"

class PdfFile : public ImageFile
{
public:
    ~PdfFile();

    const std::string &path() const;
    BasicImage *loadImage(int index) const;

    THREAD_SAFE_FUNC static bool isOpenable(const std::string &ext);
    THREAD_SAFE_FUNC static bool open(const std::string &path,
            const RawData &data, std::vector<ImageItem*> &items);
    static const StringVec &extList();

private:
    const std::string file_path;
    const RawData data;

    explicit PdfFile(const std::string &path, const RawData &data);
};

#endif // PDFFILE_HPP
