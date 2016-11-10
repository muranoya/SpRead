#ifndef STDIMGFILE
#define STDIMGFILE

#include "SpRead.hpp"
#include "ImageFile.hpp"

class StdImgFile : public ImageFile
{
public:
    ~StdImgFile();

    const std::string &path() const;
    // Param: index is ignore in this class
    BasicImage *loadImage(int index) const;

    THREAD_SAFE_FUNC static bool isOpenable(const std::string &ext);
    THREAD_SAFE_FUNC static bool open(const std::string &path,
            const RawData &data, std::vector<ImageItem*> &items);
    static const StringVec &extList();

private:
    const std::string file_path;
    const RawData data;

    explicit StdImgFile(const std::string &path, const RawData &data);
};

#endif // STDIMGFILE
