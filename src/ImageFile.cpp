#include <FL/filename.H>
#include <FL/fl_utf8.h>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include "ImageFile.hpp"
#include "ArchiveFile.hpp"
#include "StdImgFile.hpp"
#include "PdfFile.hpp"

using namespace std;

const std::vector<ImageFile::FileInfo> ImageFile::imgs = {
    {
        StdImgFile::isOpenable,
        StdImgFile::open,
        StdImgFile::extList
    }
};
const std::vector<ImageFile::FileInfo> ImageFile::docs = {
    {
        PdfFile::isOpenable,
        PdfFile::open,
        PdfFile::extList
    }
};
const std::vector<ImageFile::FileInfo> ImageFile::archs = {
    {
        ArchiveFile::isOpenable,
        ArchiveFile::open,
        ArchiveFile::extList
    }
};

ImageFile::~ImageFile()
{
}

bool
ImageFile::open(const string &path, vector<ImageItem*> &items)
{
    string ext = getExtension(path);
    if (ext.empty()) return false;

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
                fi.open(path, readFile(path), items);
                return true;
            }
        }
    }
    return false;
}

bool
ImageFile::open(const string &path, const RawData &data,
        vector<ImageItem*> &items)
{
    string ext = getExtension(path);
    if (ext.empty()) return false;

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
                fi.open(path, data, items);
                return true;
            }
        }
    }
    return false;
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
                extlist += "}\n";
            }
        }
    }
    return extlist;
}

ImageFile::RawData
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

string
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

BasicImage *
ImageItem::image() const
{
    return file->loadImage(page);
}

