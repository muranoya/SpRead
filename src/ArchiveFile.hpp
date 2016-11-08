#ifndef ARCHIVEFILE_HPP
#define ARCHIVEFILE_HPP

#include "ImageFile.hpp"

class ArchiveFile : private Uncopyable
{
public:
    static bool isOpenable(const std::string &ext);
    static bool open(const std::string &path, const ImageFile::RawData &data,
            std::vector<ImageItem*> &items);
    static const std::vector<std::string> &extList();

private:
    explicit ArchiveFile();
};

#endif // ARCHIVEFILE_HPP
