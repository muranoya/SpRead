#ifndef IMAGEFILE_HPP
#define IMAGEFILE_HPP

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "BasicImage.hpp"
#include "Uncopyable.hpp"

class ImageItem;

class ImageFile : private Uncopyable
{
public:
    typedef std::vector<uchar> RawData;
    virtual ~ImageFile();
    
    virtual const std::string &path() const = 0;
    virtual BasicImage *loadImage(int index) const = 0;

    static bool open(const std::string &path,
            std::vector<ImageItem*> &items);
    static bool open(const std::string &path,
            const RawData &data,
            std::vector<ImageItem*> &items);
    static const std::string &readableFormatExtList();

private:
    struct FileInfo
    {
        const std::function<
            bool(const std::string&)> openable;
        const std::function<
            bool(const std::string&, const RawData&,
                    std::vector<ImageItem*>&)> open;
        const std::function<
            const std::vector<std::string> &(void)> enum_exts;
    };
    static const std::vector<FileInfo> imgs;
    static const std::vector<FileInfo> docs;
    static const std::vector<FileInfo> archs;

    static RawData readFile(const std::string &path);
    static std::string getExtension(const std::string &path);
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
