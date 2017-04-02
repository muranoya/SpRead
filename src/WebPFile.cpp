#include <FL/filename.H>
#ifdef SUPPORT_WEBP
#include <webp/decode.h>
#endif
#include <algorithm>
#include "WebPFile.hpp"

using namespace std;

static const StringVec exts = {
#ifdef SUPPORT_WEBP
    "webp",
#endif
};

WebPFile::WebPFile(const string &path, const RawData &data)
    : file_path(path)
    , data(data)
{
}

WebPFile::~WebPFile()
{
}

const string &
WebPFile::path() const
{
    return file_path;
}

shared_ptr<BasicImage>
WebPFile::loadImage(int)
{
#ifdef SUPPORT_WEBP
    string ext = fl_filename_ext(file_path.c_str());
    if (ext.empty()) return nullptr;
    ext.erase(0, 1); // erase a period
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "webp")
    {
        int w, h;
        if (WebPGetInfo(data.data(), data.size(), &w, &h))
        {
            uint8_t *out;
            out = WebPDecodeRGBA(data.data(), data.size(), &w, &h);
            BasicImage *bi = new BasicImage(w, h, 4, out);
            WebPFree(out);
            return shared_ptr<BasicImage>(bi);
        }
    }
#endif
    return nullptr;
}

THREAD_SAFE_FUNC bool
WebPFile::isOpenable(const string &ext)
{
#ifdef SUPPORT_WEBP
    return any_of(exts.cbegin(), exts.cend(),
            [&ext](const string &x){ return x == ext; });
#else
    return false;
#endif
}

THREAD_SAFE_FUNC bool
WebPFile::open_webp(const string &path, const RawData &data,
        vector<ImageItem*> &items)
{
#ifdef SUPPORT_WEBP
    if (data.empty()) return false;
    shared_ptr<ImageFile> f(new WebPFile(path, data));
    ImageItem *item = new ImageItem(f);
    items.push_back(item);
    return true;
#else
    return false;
#endif
}

const StringVec &
WebPFile::extList()
{
    return exts;
}

