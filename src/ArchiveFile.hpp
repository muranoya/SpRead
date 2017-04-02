#ifndef ARCHIVEFILE_HPP
#define ARCHIVEFILE_HPP

#include "SpRead.hpp"
#include "ImageFile.hpp"

class ArchiveFile : private Uncopyable
{
public:
    THREAD_SAFE_FUNC static bool isOpenable(const std::string &ext);
    THREAD_SAFE_FUNC static bool open_arch(const std::string &path,
            const RawData &data, std::vector<ImageItem*> &items);
    static const StringVec &extList();
    void remove_from_playlist(int page) noexcept;

private:
    explicit ArchiveFile();
};

#endif // ARCHIVEFILE_HPP
