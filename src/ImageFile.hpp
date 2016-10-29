#ifndef IMAGEFILE_HPP
#define IMAGEFILE_HPP

#include <string>
#include <vector>

class ImageFile
{
public:
    enum FileType
    {
        TYPE_INVALID,
        TYPE_RAW,
        TYPE_ARCHIVE,
    };
    enum ImageFormat
    {
        IMG_INVALID,
        IMG_JPEG,
        IMG_PNG,
    };

    explicit ImageFile();
    explicit ImageFile(const std::string &path,
            const std::vector<char> &entry);
    explicit ImageFile(const std::string &path);
    ImageFile(const ImageFile &other);
    ImageFile(ImageFile &&other);
    virtual ~ImageFile();

    ImageFile &operator=(const ImageFile &other);
    ImageFile &operator=(ImageFile &&other);

    FileType fileType() const;
    ImageFormat format() const;
    bool isValid() const;
    
    std::string physicalFilePath() const;
    std::string physicalFileName() const;
    std::string logicalFilePath() const;
    std::string logicalFileName() const;

    const std::vector<char> &rawFilePath() const;
    std::string createKey() const;

    std::vector<unsigned char> *readData() const;

    static bool isReadableImageFile(const std::string &path);
    static bool isReadableArchiveFile(const std::string &path);
    static const std::string &readableFormatExt();
    static std::vector<ImageFile*> openArchive(const std::string &path);

private:
    FileType ftype;
    ImageFormat imgfmt;
    std::string file_path;
    std::string archive_path;
    std::vector<char> raw_file_entry;

    std::vector<unsigned char> *readImageData() const;
    std::vector<unsigned char> *readArchiveData() const;

    static ImageFormat getImageFormat(const std::string &path);
};

#endif // IMAGEFILE_HPP
