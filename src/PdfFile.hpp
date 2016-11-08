#ifndef PDFFILE_HPP
#define PDFFILE_HPP

#include "ImageFile.hpp"

class PdfFile : public ImageFile
{
public:
    ~PdfFile();

    const std::string &path() const;
    BasicImage *loadImage(int index) const;

    static bool isOpenable(const std::string &ext);
    static bool open(const std::string &path, const RawData &data,
            std::vector<ImageItem*> &items);
    static const std::vector<std::string> &extList();

private:
    const std::string file_path;
    const RawData data;

    explicit PdfFile(const std::string &path, const RawData &data);
};

#endif // PDFFILE_HPP
