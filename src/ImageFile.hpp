#ifndef IMAGEFILE_HPP
#define IMAGEFILE_HPP

#include <memory>
#include <functional>
#include "BasicImage.hpp"
#include "Uncopyable.hpp"
#include "SpRead.hpp"

class ImageItem;

class ImageFile : private Uncopyable
{
public:
    enum OpenResult
    {
        OpenError,
        OpenSuccess,
        NoCompatible,
    };
    virtual ~ImageFile();
    
    virtual const std::string &path() const = 0;
    virtual std::shared_ptr<BasicImage> loadImage(int index) = 0;

    THREAD_SAFE_FUNC static OpenResult open(const std::string &path,
            std::vector<ImageItem*> &items);
    THREAD_SAFE_FUNC static OpenResult open(const std::string &path,
            const RawData &data, std::vector<ImageItem*> &items);
    static const std::string &readableFormatExtList();

private:
    struct FileInfo
    {
        THREAD_SAFE_FUNC const std::function<
            bool(const std::string&)> openable;
        THREAD_SAFE_FUNC const std::function<
            bool(const std::string&, const RawData&,
                    std::vector<ImageItem*>&)> open;
        const std::function<
            const StringVec &(void)> enum_exts;
    };
    static const std::vector<FileInfo> imgs;
    static const std::vector<FileInfo> docs;
    static const std::vector<FileInfo> archs;

    THREAD_SAFE_FUNC static RawData readFile(const std::string &path);
    THREAD_SAFE_FUNC static std::string getExtension(const std::string &path);
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

    std::shared_ptr<BasicImage> image() const;

private:
    const int page;
    const std::string entry_name;
    const std::shared_ptr<ImageFile> file;
};

#endif // IMAGEFILE_HPP
