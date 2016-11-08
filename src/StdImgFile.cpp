#include <FL/filename.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_PNG_Image.H>
#include <algorithm>
#include "StdImgFile.hpp"

using namespace std;

static const vector<string> exts = {
    "jpg", "jpeg",
    "png",
};

StdImgFile::StdImgFile(const string &path, const RawData &data)
    : file_path(path)
    , data(data)
{
}

StdImgFile::~StdImgFile()
{
}

const string &
StdImgFile::path() const
{
    return file_path;
}

BasicImage *
StdImgFile::loadImage(int index) const
{
    string ext = fl_filename_ext(file_path.c_str());
    if (ext.empty()) return nullptr;
    ext.erase(0, 1); // erase a period
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "jpg" || ext == "jpeg")
    {
        return new BasicImage(Fl_JPEG_Image(nullptr, data.data()));
    }
    if (ext == "png")
    {
        return new BasicImage(
                Fl_PNG_Image(nullptr, data.data(), data.size()));
    }
    return nullptr;
}

bool
StdImgFile::isOpenable(const string &ext)
{
    return any_of(exts.cbegin(), exts.cend(),
            [&ext](const string &x){ return x == ext; });
}

bool
StdImgFile::open(const string &path, const RawData &data,
        vector<ImageItem*> &items)
{
    if (data.empty()) return false;
    shared_ptr<ImageFile> f(new StdImgFile(path, data));
    ImageItem *item = new ImageItem(f);
    items.push_back(item);
    return true;
}

const vector<string> &
StdImgFile::extList()
{
    return exts;
}

