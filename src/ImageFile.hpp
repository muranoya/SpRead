#ifndef IMAGEFILE_HPP
#define IMAGEFILE_HPP

#include <string>
#include <vector>
#include <memory>
#include "BasicImage.hpp"
#include "Uncopyable.hpp"

class ImageItem;

class ImageFile : private Uncopyable
{
public:
    static ImageFile *create(const std::string &path,
            const std::vector<uchar> &data,
            const std::string &entry = std::string());
    ~ImageFile();

    const std::string &path() const;
    BasicImage *loadImage(int index) const;

    static bool open(const std::string &path,
            std::vector<ImageItem*> &items);
    static bool isReadableImage(const std::string &path);
    static bool isReadableArchive(const std::string &path);
    static bool isReadableDocument(const std::string &path);
    static const std::string &readableFormatExt();

private:
    enum FileType
    {
        FT_INVALID,
        FT_IMAGE,
        FT_ARCHIVE,
#ifdef SUPPORT_PDF
        FT_PDF,
#endif
    };
    const FileType ftype;
    const std::string file_path;
    const std::string entry_name;
    const std::vector<uchar> data;

    explicit ImageFile(FileType ft,
            const std::string &path,
            const std::string &entry,
            const std::vector<uchar> &data);

    BasicImage *loadImageFromArchive(int index) const;
#ifdef SUPPORT_PDF
    BasicImage *loadImageFromPDF(int page) const;
#endif

    static std::vector<uchar> readFile(const std::string &path);
    static bool openImage(const std::string &path,
            const std::vector<uchar> &data,
            std::vector<ImageItem*> &items);
#ifdef SUPPORT_PDF
    static bool openPdf(const std::string &path,
            const std::vector<uchar> &data,
            std::vector<ImageItem*> &items);
#endif
    static bool openArchive(const std::string &path,
            const std::vector<uchar> &data,
            std::vector<ImageItem*> &items);

    static BasicImage *convert(const std::string &path,
            const std::vector<uchar> &data);
    static bool isReadable(const std::string &path,
            const std::vector<std::string> &extvec);
};

class ImageItem : private Uncopyable
{
public:
    explicit ImageItem(std::shared_ptr<ImageFile> file,
            int index = -1,
            const std::string &entry_name = std::string());
    ~ImageItem();

    std::string physicalPath() const;
    std::string physicalName() const;
    std::string virtualPath() const;
    std::string virtualName() const;

    BasicImage *image() const;

private:
    const int page;
    const std::string entry_name;
    const std::shared_ptr<ImageFile> file;
};

#endif // IMAGEFILE_HPP
