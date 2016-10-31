#include <FL/filename.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_PNG_Image.H>
#include <utility>
#include <cstring>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <array>
#include <archive.h>
#include <archive_entry.h>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-page-renderer.h>
#include <poppler/cpp/poppler-image.h>
#include "ImageFile.hpp"

using namespace std;

// When you add/remove extensions to this array,
// you have to change the ImageFile::getImageFormat().
// Following arrays strings must be consisted
// only lower-case.
static const string readable_image[] = 
{
    "jpg", "jpeg",
    "png",
};

static const string readable_doc[] = 
{
    "pdf",
};

static const string readable_archive[] =
{
    "zip", "tar", "7z", "cab", "rar", "lha", "lzh",
};

ImageFile::ImageFile()
    : ftype(TYPE_INVALID)
    , imgfmt(IMG_INVALID)
    , file_path()
    , archive_path()
    , raw_file_entry()
    , page_idx(-1)
{
}

ImageFile::ImageFile(const string &path,
        const std::vector<char> &entry)
    : ftype(TYPE_ARCHIVE)
    , imgfmt(IMG_INVALID)
    , file_path(path + "/" + string(entry.begin(), entry.end()))
    , archive_path(path)
    , raw_file_entry(entry)
    , page_idx(-1)
{
    imgfmt = getImageFormat(file_path);
    raw_file_entry.shrink_to_fit();
}

ImageFile::ImageFile(const string &imagefile)
    : ftype(TYPE_IMG)
    , imgfmt(IMG_INVALID)
    , file_path(imagefile)
    , archive_path()
    , raw_file_entry()
    , page_idx(-1)
{
    imgfmt = getImageFormat(imagefile);
}

ImageFile::ImageFile(const string &path, int page)
    : ftype(TYPE_PDF)
    , imgfmt(IMG_INVALID)
    , file_path(path)
    , archive_path()
    , raw_file_entry()
    , page_idx(page)
{
}

ImageFile::ImageFile(const ImageFile &other)
    : ftype(other.ftype)
    , imgfmt(other.imgfmt)
    , file_path(other.file_path)
    , archive_path(other.archive_path)
    , raw_file_entry(other.raw_file_entry)
    , page_idx(other.page_idx)
{
}

ImageFile::ImageFile(ImageFile &&other)
    : ftype(other.ftype)
    , imgfmt(other.imgfmt)
    , file_path(std::move(other.file_path))
    , archive_path(std::move(other.archive_path))
    , raw_file_entry(std::move(other.raw_file_entry))
    , page_idx(other.page_idx)
{
}

ImageFile::~ImageFile()
{
}

ImageFile &
ImageFile::operator=(const ImageFile &other)
{
    ftype = other.ftype;
    imgfmt = other.imgfmt;
    file_path = other.file_path;
    archive_path = other.archive_path;
    raw_file_entry = other.raw_file_entry;
    page_idx = other.page_idx;
    return *this;
}

ImageFile &
ImageFile::operator=(ImageFile &&other)
{
    ftype = other.ftype;
    imgfmt = other.imgfmt;
    file_path = std::move(other.file_path);
    archive_path = std::move(other.archive_path);
    raw_file_entry = std::move(other.raw_file_entry);
    page_idx = other.page_idx;
    return *this;
}

ImageFile::ImageFormat
ImageFile::format() const
{
    return imgfmt;
}

string
ImageFile::physicalFilePath() const
{
    switch (ftype)
    {
        case TYPE_INVALID: break;
        case TYPE_IMG:     return file_path;
        case TYPE_ARCHIVE: return archive_path;
        case TYPE_PDF:     return file_path;
    }
    return string();
}

string
ImageFile::physicalFileName() const
{
    return string(fl_filename_name(physicalFilePath().c_str()));
}

string
ImageFile::logicalFilePath() const
{
    switch (ftype)
    {
        case TYPE_INVALID: break;
        case TYPE_IMG:     return file_path;
        case TYPE_ARCHIVE: return file_path;
        case TYPE_PDF:     return file_path + "/" + to_string(page_idx);
    }
    return string();
}

string
ImageFile::logicalFileName() const
{
    return string(fl_filename_name(logicalFilePath().c_str()));
}

string
ImageFile::createKey() const
{
    return logicalFilePath();
}

BasicImage *
ImageFile::image() const
{
    vector<unsigned char> *d;
    switch (ftype)
    {
        case TYPE_IMG:
            d = readImageData();
            break;
        case TYPE_ARCHIVE:
            d = readArchiveData();
            break;
        case TYPE_PDF:
            return readPdfData();
        default:
            return nullptr;
    }
    if (!d) return nullptr;

    BasicImage *img = nullptr;
    switch (imgfmt)
    {
        case IMG_JPEG:
            img = new BasicImage(Fl_JPEG_Image(nullptr, d->data()));
            break;
        case IMG_PNG:
            img = new BasicImage(Fl_PNG_Image(nullptr, d->data(),
                        d->size()));
            break;
        default:
            break;
    }
    delete d;
    return img;
}

void
ImageFile::open(const string &path, vector<ImageFile*> &lists)
{
    if (isReadableImage(path))
    {
        ImageFile *f = new ImageFile(path);
        lists.push_back(f);
    }
    else if (isReadableDocument(path))
    {
        poppler::document *doc;
        doc = poppler::document::load_from_file(path);
        if (!doc || doc->is_encrypted() || doc->is_locked())
        {
            // error
            delete doc;
            return;
        }

        int page_num = doc->pages();
        delete doc;
        for (int i = 0; i < page_num; ++i)
        {
            ImageFile *f = new ImageFile(path, i);
            lists.push_back(f);
        }
    }
    else
    {
        if (!isReadableArchive(path)) return;

        struct archive *a = archive_read_new();
        archive_read_support_filter_all(a);
        archive_read_support_format_all(a);
        int r = archive_read_open_filename(
                a, path.c_str(), 512*1024);
        if (r != ARCHIVE_OK)
        {
            cerr << archive_error_string(a) << endl;
            archive_read_free(a);
            return;
        }

        struct archive_entry *ae;
        while (archive_read_next_header(a, &ae) == ARCHIVE_OK)
        {
            const char *raw_entry = archive_entry_pathname(ae);
            string entry_name(raw_entry);
            if (ImageFile::isReadableImage(entry_name))
            {
                archive_read_data_skip(a);
                vector<char> re(raw_entry,
                        raw_entry + entry_name.size());
                lists.push_back(new ImageFile(path, re));
            }
        }

        r = archive_read_free(a);
        if (r != ARCHIVE_OK)
        {
            cerr << archive_error_string(a) << endl;
        }
    }
}

const string &
ImageFile::readableFormatExt()
{
    static string extlist;
    if (extlist.empty())
    {
        int i, len;

        len = sizeof(readable_image) / 
            sizeof(readable_image[0]);
        extlist = "Images\t*.{";
        for (i = 0; i < len-1; ++i)
        {
            extlist += readable_image[i];
            extlist += ",";
        }
        extlist += readable_image[i] + "}\n";

        len = sizeof(readable_doc) / 
            sizeof(readable_doc[0]);
        extlist += "Document\t*.{";
        for (i = 0; i < len-1; ++i)
        {
            extlist += readable_image[i];
            extlist += ",";
        }
        extlist += readable_doc[i] + "}\n";

        len = sizeof(readable_archive) / 
            sizeof(readable_archive[0]);
        extlist += "Archives\t*.{";
        for (i = 0; i < len-1; ++i)
        {
            extlist += readable_archive[i];
            extlist += ",";
        }
        extlist += readable_archive[i] + "}";
    }
    return extlist;
}

vector<unsigned char> *
ImageFile::readImageData() const
{
    ifstream rfile;
    rfile.open(physicalFilePath(), ifstream::in | ifstream::binary);

    if (!rfile.fail())
    {
        auto fs_s = rfile.seekg(0, ios::beg).tellg();
        rfile.clear();
        auto fs_e = rfile.seekg(0, ios::end).tellg();
        rfile.clear();
        auto fsize = fs_e - fs_s;
        rfile.seekg(0, ios::beg).clear();

        vector<unsigned char> *vec = new vector<unsigned char>(fsize);
        rfile.read(reinterpret_cast<char*>(vec->data()), fsize);

        return vec;
    }
    return nullptr;
}

vector<unsigned char> *
ImageFile::readArchiveData() const
{
    struct archive *a;

    a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);
    int r = archive_read_open_filename(a,
            physicalFilePath().c_str(), 512*1024);
    if (r != ARCHIVE_OK)
    {
        cerr << archive_error_string(a) << endl;
        archive_read_free(a);
        return nullptr;
    }

    struct archive_entry *ae;
    const vector<char> &s_entry = raw_file_entry;
    while (archive_read_next_header(a, &ae) == ARCHIVE_OK)
    {
        const char *entry = archive_entry_pathname(ae);
        string entry_str(entry);
        vector<char> entry_vec(entry, entry+entry_str.size());
        if (s_entry == entry_vec)
        {
            vector<unsigned char> *data = new vector<unsigned char>();
            const void *buf;
            size_t len;
            la_int64_t offset;

            while (archive_read_data_block(a, &buf, &len, &offset)
                    == ARCHIVE_OK)
            {
                int e1 = data->size();
                data->resize(data->size() + len);
                for (size_t i = 0; i < len; ++i)
                {
                    unsigned char c = static_cast<const unsigned char*>(buf)[i];
                    (*data)[e1+i] = c;
                }
            }
            archive_read_free(a);
            if (data->empty())
            {
                delete data;
                return nullptr;
            }
            else
            {
                return data;
            }
        }
        else
        {
            archive_read_data_skip(a);
        }
    }
    archive_read_free(a);
    return nullptr;
}

BasicImage *
ImageFile::readPdfData() const
{
    poppler::document *doc;
    doc = poppler::document::load_from_file(physicalFilePath());
    if (!doc || doc->is_encrypted() || doc->is_locked())
    {
        // error
        delete doc;
        return nullptr;
    }

    poppler::page *p = doc->create_page(page_idx);
    if (!p)
    {
        // error
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
            delete doc;
            delete p;
            return nullptr;
    }

    BasicImage *bimg = new BasicImage(img.width(), img.height(), d,
            reinterpret_cast<const unsigned char*>(img.const_data()));
    delete doc;
    delete p;
    return bimg;
}

bool
ImageFile::isReadableImage(const string &path)
{
    int len = sizeof(readable_image) / 
        sizeof(readable_image[0]);
    return isReadable(path, readable_image, len);
}

bool
ImageFile::isReadableArchive(const string &path)
{
    int len = sizeof(readable_archive) / 
        sizeof(readable_archive[0]);
    return isReadable(path, readable_archive, len);
}

bool
ImageFile::isReadableDocument(const string &path)
{
    int len = sizeof(readable_doc) / 
        sizeof(readable_doc[0]);
    return isReadable(path, readable_doc, len);
}

bool
ImageFile::isReadable(const string &path, const string exts[],
        int len)
{
    string ext = string(fl_filename_ext(path.c_str()));
    if (ext.empty()) return false;
    ext.erase(0, 1); //erase a period
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    for (int i = 0; i < len; ++i)
    {
        if (ext == exts[i]) return true;
    }
    return false;
}

ImageFile::ImageFormat
ImageFile::getImageFormat(const string &path)
{
    string ext = fl_filename_ext(path.c_str());
    if (ext.empty()) return IMG_INVALID;
    ext.erase(0, 1); // erase a period
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "jpg" || ext == "jpeg") return IMG_JPEG;
    if (ext == "png"                 ) return IMG_PNG;
    return IMG_INVALID;
}

