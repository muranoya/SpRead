#ifndef WEBPFILE
#define WEBPFILE

#include "SpRead.hpp"
#include "ImageFile.hpp"

class WebPFile : public ImageFile
{
public:
    ~WebPFile();

    const std::string &path() const;
    // Param: index is ignore in this class
    std::shared_ptr<BasicImage> loadImage(int index);

    THREAD_SAFE_FUNC static bool isOpenable(const std::string &ext);
    THREAD_SAFE_FUNC static bool open_webp(const std::string &path,
            const RawData &data, std::vector<ImageItem*> &items);
    static const StringVec &extList();

private:
    const std::string file_path;
    const RawData data;

    explicit WebPFile(const std::string &path, const RawData &data);
};

#endif // WEBPFILE
