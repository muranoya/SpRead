#ifndef STDIMGFILE
#define STDIMGFILE

#include "ImageFile.hpp"

class StdImgFile : public ImageFile
{
public:
    ~StdImgFile();

    const std::string &path() const;
    // Param: index is ignore in this class
    BasicImage *loadImage(int index) const;

    static bool isOpenable(const std::string &ext);
    static bool open(const std::string &path, const RawData &data,
            std::vector<ImageItem*> &items);
    static const std::vector<std::string> &extList();

private:
    const std::string file_path;
    const RawData data;

    explicit StdImgFile(const std::string &path, const RawData &data);
};

#endif // STDIMGFILE
