#ifndef PDFFILE_HPP
#define PDFFILE_HPP

#include "ImageFile.hpp"
#ifdef SUPPORT_PDF
#include <poppler/cpp/poppler-document.h>
#endif

class PdfFile : public ImageFile
{
public:
    ~PdfFile();

    const std::string &path() const;
    std::shared_ptr<BasicImage> loadImage(int page);

    THREAD_SAFE_FUNC static bool isOpenable(const std::string &ext);
    THREAD_SAFE_FUNC static bool open_pdf(const std::string &path,
            const RawData &data, std::vector<ImageItem*> &items);
    static const StringVec &extList();

private:
    const std::string file_path;
    const RawData data;
#ifdef SUPPORT_PDF
    poppler::document *doc;
#endif

    explicit PdfFile(const std::string &path, const RawData &data);
    static BasicImage *rendering_page(poppler::document *doc, int page);
};

#endif // PDFFILE_HPP
