#include <iostream>
#include <algorithm>
#include <archive.h>
#include <archive_entry.h>
#include "ArchiveFile.hpp"

using namespace std;

static const StringVec exts = {
    "zip", "tar", "7z", "cab",
    "rar", "lha", "lzh",
};

THREAD_SAFE_FUNC bool
ArchiveFile::isOpenable(const string &ext)
{
    return any_of(exts.cbegin(), exts.cend(),
            [&ext](const string &x) { return x == ext; });
}

THREAD_SAFE_FUNC bool
ArchiveFile::open_arch(const string &path, const RawData &data,
        vector<ImageItem*> &items)
{
    struct archive *a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);
    int r = archive_read_open_memory(a, data.data(), data.size());
    if (r != ARCHIVE_OK)
    {
        cerr << archive_error_string(a) << endl;
        archive_read_free(a);
        return false;
    }

    struct archive_entry *ae;
    while (archive_read_next_header(a, &ae) == ARCHIVE_OK)
    {
        string entry_name = archive_entry_pathname(ae);
        RawData data;
        const void *buf;
        size_t len;
        la_int64_t offset;

        while (archive_read_data_block(a, &buf, &len, &offset) == ARCHIVE_OK)
        {
            int e1 = data.size();
            data.resize(e1 + len);
            for (size_t i = 0; i < len; ++i)
            {
                data[e1+i] = static_cast<const uchar*>(buf)[i];
            }
        }
        ImageFile::open(path + "/" + entry_name, data, items);
        archive_read_data_skip(a);
    }
    archive_read_free(a);
    return true;
}

void
ArchiveFile::remove_from_playlist(int) noexcept
{
}

const StringVec &
ArchiveFile::extList()
{
    return exts;
}

