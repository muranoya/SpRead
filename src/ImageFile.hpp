#ifndef IMAGEFILE_HPP
#define IMAGEFILE_HPP

#include <string>
#include <vector>
#include "BasicImage.hpp"

class ImageFile
{
public:
    enum FileType
    {
        TYPE_INVALID,
        TYPE_IMG,
        TYPE_ARCHIVE,
        TYPE_PDF,
    };
    enum ImageFormat
    {
        IMG_INVALID,
        IMG_JPEG,
        IMG_PNG,
    };

    explicit ImageFile();
    // for archive
    explicit ImageFile(const std::string &path,
            const std::vector<char> &entry);
    // for image
    explicit ImageFile(const std::string &path);
    // for pdf
    explicit ImageFile(const std::string &path,
            int page);
    ImageFile(const ImageFile &other);
    ImageFile(ImageFile &&other);
    virtual ~ImageFile();

    ImageFile &operator=(const ImageFile &other);
    ImageFile &operator=(ImageFile &&other);

    ImageFormat format() const;
    
    std::string physicalFilePath() const;
    std::string physicalFileName() const;
    std::string logicalFilePath() const;
    std::string logicalFileName() const;
    std::string createKey() const;

    BasicImage *image() const;

    static void open(const std::string &path,
            std::vector<ImageFile*> &lists);
    static const std::string &readableFormatExt();

private:
    FileType ftype;
    ImageFormat imgfmt;
    std::string file_path;
    std::string archive_path;
    std::vector<char> raw_file_entry;
    int page_idx;

    std::vector<unsigned char> *readImageData() const;
    std::vector<unsigned char> *readArchiveData() const;
    BasicImage *readPdfData() const;

    static bool isReadableImage(const std::string &path);
    static bool isReadableArchive(const std::string &path);
    static bool isReadableDocument(const std::string &path);
    static bool isReadable(const std::string &path,
            const std::string exts[], int len);
    static ImageFormat getImageFormat(const std::string &path);
};

#endif // IMAGEFILE_HPP
