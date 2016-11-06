#include <FL/filename.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_PNG_Image.H>
#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <archive.h>
#include <archive_entry.h>
#ifdef SUPPORT_PDF
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-page-renderer.h>
#include <poppler/cpp/poppler-image.h>
#endif
#include "ImageFile.hpp"

using namespace std;

// When you add/remove extensions to this array,
// you have to change the ImageFile::convert().
// Following arrays strings must be consisted
// only lower-case.
static const vector<string> readable_image =
{
    "jpg", "jpeg",
    "png",
};
#ifdef SUPPORT_PDF
static const vector<string> readable_doc =
{
    "pdf",
};
#endif
static const vector<string> readable_archive =
{
    "zip", "tar", "7z", "cab", "rar", "lha", "lzh",
};

ImageFile *
ImageFile::create(const string &path,
        const vector<uchar> &data, const string &entry)
{
    if (data.empty()) return nullptr;

    FileType ft = FT_INVALID;
    if (isReadableImage(path))
    {
        ft = FT_IMAGE;
    }
    else if (isReadableArchive(path))
    {
        ft = FT_ARCHIVE;
    }
#ifdef SUPPORT_PDF
    else if (isReadableDocument(path))
    {
        ft = FT_PDF;
    }
#endif
    else
    {
        return nullptr;
    }
    return new ImageFile(ft, path, entry, data);
}

ImageFile::ImageFile(FileType ft, const string &path, const string &entry,
        const vector<uchar> &data)
    : ftype(ft)
    , file_path(path)
    , entry_name(entry)
    , data(data)
{
}

ImageFile::~ImageFile()
{
}

const string &
ImageFile::path() const
{
    return file_path;
}

BasicImage *
ImageFile::loadImage(int index) const
{
    switch (ftype)
    {
        case FT_IMAGE:   return convert(file_path, data);
        case FT_ARCHIVE: return loadImageFromArchive(index);
#ifdef SUPPORT_PDF
        case FT_PDF:     return loadImageFromPDF(index);
#endif
        case FT_INVALID: break;
    }
    return nullptr;
}

bool
ImageFile::open(const string &path, vector<ImageItem*> &items)
{
    if (isReadableImage(path))
        return openImage(path, readFile(path), items);
#ifdef SUPPORT_PDF
    if (isReadableDocument(path))
        return openPdf(path, readFile(path), items);
#endif
    if (isReadableArchive(path))
        return openArchive(path, readFile(path), items);
    return false;
}

bool
ImageFile::isReadableImage(const string &path)
{
    return isReadable(path, readable_image);
}

bool
ImageFile::isReadableArchive(const string &path)
{
    return isReadable(path, readable_archive);
}

bool
ImageFile::isReadableDocument(const string &path)
{
#ifdef SUPPORT_PDF
    return isReadable(path, readable_doc);
#else
    return false;
#endif
}

const string &
ImageFile::readableFormatExt()
{
    static string extlist;
    if (extlist.empty())
    {
        extlist = "Images\t*.{";
        for_each(readable_image.cbegin(), readable_image.cend(),
                 [](const string &x) { extlist += x + ","; });
        extlist += "}\n";

        extlist += "Archives\t*.{";
        for_each(readable_archive.cbegin(), readable_archive.cend(),
                 [](const string &x) { extlist += x + ","; });
        extlist += "}\n";

#ifdef SUPPORT_PDF
        extlist += "Document\t*.{";
        for_each(readable_doc.cbegin(), readable_doc.cend(),
                 [](const string &x) { extlist += x + ","; });
        extlist += "}";
#endif
    }
    return extlist;
}

BasicImage *
ImageFile::loadImageFromArchive(int index) const
{
    if (ftype != FT_ARCHIVE) return nullptr;

    struct archive *a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);
    int r = archive_read_open_memory(a, data.data(), data.size());
    if (r != ARCHIVE_OK)
    {
        cerr << archive_error_string(a) << endl;
        archive_read_free(a);
        return nullptr;
    }

    struct archive_entry *ae;
    BasicImage *img = nullptr;
    int i = 0;
    while (archive_read_next_header(a, &ae) == ARCHIVE_OK)
    {
        if (i == index)
        {
            vector<uchar> imgdata;
            const void *buf;
            size_t len;
            la_int64_t offset;
            while (archive_read_data_block(a, &buf, &len, &offset)
                    == ARCHIVE_OK)
            {
                int e1 = imgdata.size();
                imgdata.resize(e1 + len);
                for (size_t i = 0; i < len; ++i)
                {
                    imgdata[e1+i] = static_cast<const uchar*>(buf)[i];
                }
            }
            if (!imgdata.empty())
            {
                string image_name = archive_entry_pathname(ae);
                img = convert(image_name, imgdata);
            }
            break;
        }
        else
        {
            i++;
            archive_read_data_skip(a);
        }
    }
    archive_read_free(a);
    return img;
}

#ifdef SUPPORT_PDF
BasicImage *
ImageFile::loadImageFromPDF(int page) const
{
    if (ftype != FT_PDF) return nullptr;

    poppler::document *doc;
    doc = poppler::document::load_from_raw_data(
            reinterpret_cast<const char*>(data.data()), data.size());
    if (!doc || doc->is_encrypted() || doc->is_locked())
    {
        cerr << "Cannot load " << file_path << " : " << entry_name << endl;
        delete doc;
        return nullptr;
    }

    poppler::page *p = doc->create_page(page);
    if (!p)
    {
        cerr << "Cannot load a page of PDF : " << page << endl;
        delete p;
        return nullptr;
    }
    poppler::page_renderer renderer;
    poppler::image img = renderer.render_page(p, 144.0, 144.0);
    int d;
    switch (img.format())
    {
        case poppler::image::format_mono:   d = 1; break;
        case poppler::image::format_rgb24:  d = 3; break;
        case poppler::image::format_argb32: d = 4; break; 
        default:
            cerr << "Unknown image depth of PDF on page " << page << endl;
            delete doc;
            delete p;
            return nullptr;
    }

    BasicImage *bimg = new BasicImage(img.width(), img.height(), d,
            reinterpret_cast<const uchar*>(img.const_data()));
    delete doc;
    delete p;
    return bimg;
}
#endif

vector<uchar>
ImageFile::readFile(const string &path)
{
    vector<uchar> vec;
    ifstream rfile;
    rfile.open(path, ifstream::in | ifstream::binary);
    if (rfile.fail()) return vec;

    auto fs_s = rfile.seekg(0, ios::beg).tellg(); rfile.clear();
    auto fs_e = rfile.seekg(0, ios::end).tellg(); rfile.clear();
    auto fsize = fs_e - fs_s;
    rfile.seekg(0, ios::beg).clear();

    vec.resize(fsize);
    rfile.read(reinterpret_cast<char*>(vec.data()), fsize);
    rfile.close();
    return vec;
}

bool
ImageFile::openImage(const string &path, const vector<uchar> &data, vector<ImageItem*> &items)
{
    if (data.empty()) return false;
    shared_ptr<ImageFile> f(create(path, data));
    items.push_back(new ImageItem(f));
    return true;
}

#ifdef SUPPORT_PDF
bool
ImageFile::openPdf(const string &path, const vector<uchar> &data, vector<ImageItem*> &items)
{
    poppler::document *doc =
        poppler::document::load_from_raw_data(
                reinterpret_cast<const char*>(data.data()), data.size());
    if (!doc || doc->is_encrypted() || doc->is_locked())
    {
        cerr << "Cannot load PDF from memory." << endl;
        cerr << "File: " << path << endl;
        delete doc;
        return false;
    }

    int page_num = doc->pages();
    delete doc;
    shared_ptr<ImageFile> f(create(path, data));
    for (int i = 0; i < page_num; ++i)
    {
        items.push_back(new ImageItem(f, i, to_string(i)));
    }
    return true;
}
#endif

bool
ImageFile::openArchive(const string &path, const vector<uchar> &data, vector<ImageItem*> &items)
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

    shared_ptr<ImageFile> f_arc(create(path, data));
    struct archive_entry *ae;
    int index = 0;
    while (archive_read_next_header(a, &ae) == ARCHIVE_OK)
    {
        string entry_name = archive_entry_pathname(ae);
        if (isReadableImage(entry_name))
        {
            items.push_back(new ImageItem(f_arc, index, entry_name));
        }
#ifdef SUPPORT_PDF
        else if (isReadableDocument(entry_name))
        {
            vector<uchar> data;
            const void *buf;
            size_t len;
            la_int64_t offset;

            while (archive_read_data_block(a, &buf, &len, &offset)
                    == ARCHIVE_OK)
            {
                int e1 = data.size();
                data.resize(e1 + len);
                for (size_t i = 0; i < len; ++i)
                {
                    data[e1+i] = static_cast<const uchar*>(buf)[i];
                }
            }
            openPdf(path + "/" + entry_name, data, items);
        }
#endif
        archive_read_data_skip(a);
        index++;
    }
    archive_read_free(a);
    return true;
}

BasicImage *
ImageFile::convert(const string &path,
        const vector<uchar> &data)
{
    string ext = fl_filename_ext(path.c_str());
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
ImageFile::isReadable(const string &path, const vector<string> &extvec)
{
    string ext = string(fl_filename_ext(path.c_str()));
    if (ext.empty()) return false;
    ext.erase(0, 1); //erase a period
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return any_of(extvec.cbegin(), extvec.cend(),
            [&ext](string s) { return ext == s; });
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

