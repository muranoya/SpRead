#include <FL/filename.H>
#include <FL/fl_utf8.h>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include "ImageFile.hpp"
#include "ArchiveFile.hpp"
#include "StdImgFile.hpp"
#include "WebPFile.hpp"
#include "PdfFile.hpp"

using namespace std;

// Functors that register in the following vectors must be thread-safe,
// because there will be called from the Playlist::openFilesAndDirs function
// and it runs in a thread different from the main-thread.
const std::vector<ImageFile::FileInfo> ImageFile::imgs = {
    {
        StdImgFile::isOpenable,
        StdImgFile::open_stdimg,
        StdImgFile::extList
    },
    {
        WebPFile::isOpenable,
        WebPFile::open_webp,
        WebPFile::extList
    },
};
const std::vector<ImageFile::FileInfo> ImageFile::docs = {
    {
        PdfFile::isOpenable,
        PdfFile::open_pdf,
        PdfFile::extList
    },
};
const std::vector<ImageFile::FileInfo> ImageFile::archs = {
    {
        ArchiveFile::isOpenable,
        ArchiveFile::open_arch,
        ArchiveFile::extList
    },
};

ImageFile::~ImageFile()
{
}

THREAD_SAFE_FUNC ImageFile::OpenResult
ImageFile::open(const string &path, vector<ImageItem*> &items)
{
    string ext = getExtension(path);
    if (ext.empty()) return NoCompatible;

    const vector<vector<FileInfo>> files = {
        imgs,
        docs,
        archs,
    };

    for (auto f : files)
    {
        for (auto fi : f)
        {
            if (fi.openable(ext))
            {
                if (fi.open(path, readFile(path), items))
                {
                    return OpenSuccess;
                }
                else
                {
                    return OpenError;
                }
            }
        }
    }
    return NoCompatible;
}

THREAD_SAFE_FUNC ImageFile::OpenResult
ImageFile::open(const string &path, const RawData &data,
        vector<ImageItem*> &items)
{
    string ext = getExtension(path);
    if (ext.empty()) return NoCompatible;

    const vector<vector<FileInfo>> files = {
        imgs,
        docs,
        archs,
    };

    for (auto f : files)
    {
        for (auto fi : f)
        {
            if (fi.openable(ext))
            {
                if (fi.open(path, data, items))
                {
                    return OpenSuccess;
                }
                else
                {
                    return OpenError;
                }
            }
        }
    }
    return NoCompatible;
}

const string &
ImageFile::readableFormatExtList()
{
    static string extlist;
    if (extlist.empty())
    {
        const std::vector<pair<string, vector<FileInfo>>> files = {
            make_pair("Images",    imgs),
            make_pair("Documents", docs),
            make_pair("Archives",  archs),
        };

        for (auto p : files)
        {
            extlist += p.first + "\t*.{";
            for (auto x : p.second)
            {
                for (auto y : x.enum_exts())
                {
                    extlist += y + ",";
                }
            }
            extlist += "}\n";
        }
    }
    return extlist;
}

THREAD_SAFE_FUNC RawData
ImageFile::readFile(const string &path)
{
    RawData data;
    FILE *fp = fl_fopen(path.c_str(), "rb");
    if (!fp)
    {
        cerr << "Error: cannot open " << path << endl;
        return data;
    }

    fseek(fp, 0, SEEK_END);
    auto fsize = ftell(fp);
    rewind(fp);

    data.resize(fsize);
    fread(data.data(), 1, fsize, fp);
    fclose(fp);
    return data;
}

THREAD_SAFE_FUNC string
ImageFile::getExtension(const string &path)
{
    string ext = fl_filename_ext(path.c_str());
    if (ext.empty()) return string();
    ext.erase(0, 1); // erase a period
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

ImageItem::ImageItem(shared_ptr<ImageFile> file, int index,
        const string &entry_name)
    : page(index)
    , entry_name(entry_name)
    , file(file)
{
}

ImageItem::~ImageItem()
{
}

string
ImageItem::physicalPath() const
{
    return file->path();
}

string
ImageItem::physicalName() const
{
    return string(fl_filename_name(physicalPath().c_str()));
}

string
ImageItem::virtualPath() const
{
    string str = physicalPath();
    if (!entry_name.empty()) str += "/" + entry_name;
    return str;
}

string
ImageItem::virtualName() const
{
    return string(fl_filename_name(virtualPath().c_str()));
}

shared_ptr<BasicImage>
ImageItem::image() const
{
    return file->loadImage(page);
}

