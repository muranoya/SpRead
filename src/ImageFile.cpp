#include <FL/filename.H>

#include <utility>
#include <cstring>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <array>
#include <archive.h>
#include <archive_entry.h>
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
{
}

ImageFile::ImageFile(const string &path,
        const std::vector<char> &entry)
    : ftype(TYPE_ARCHIVE)
    , imgfmt()
    , file_path(path + "/" + string(entry.begin(), entry.end()))
    , archive_path(path)
    , raw_file_entry(entry)
{
    imgfmt = getImageFormat(file_path);
    raw_file_entry.shrink_to_fit();
}

ImageFile::ImageFile(const string &imagefile)
    : ftype(TYPE_RAW)
    , imgfmt()
    , file_path(imagefile)
    , archive_path()
    , raw_file_entry()
{
    imgfmt = getImageFormat(imagefile);
}

ImageFile::ImageFile(const ImageFile &other)
    : ftype(other.ftype)
    , imgfmt(other.imgfmt)
    , file_path(other.file_path)
    , archive_path(other.archive_path)
    , raw_file_entry(other.raw_file_entry)
{
}

ImageFile::ImageFile(ImageFile &&other)
    : ftype(other.ftype)
    , imgfmt(other.imgfmt)
    , file_path(std::move(other.file_path))
    , archive_path(std::move(other.archive_path))
    , raw_file_entry(std::move(other.raw_file_entry))
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
    return *this;
}

ImageFile::FileType
ImageFile::fileType() const
{
    return ftype;
}

ImageFile::ImageFormat
ImageFile::format() const
{
    return imgfmt;
}

bool
ImageFile::isValid() const
{
    return fileType() != TYPE_INVALID;
}

string
ImageFile::physicalFilePath() const
{
    switch (ftype)
    {
        case TYPE_ARCHIVE: return archive_path;
        case TYPE_RAW:     return file_path;
        case TYPE_INVALID: break;
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
        case TYPE_ARCHIVE: return file_path;
        case TYPE_RAW:     return file_path;
        case TYPE_INVALID: break;
    }
    return string();
}

string
ImageFile::logicalFileName() const
{
    return string(fl_filename_name(logicalFilePath().c_str()));
}

const vector<char> &
ImageFile::rawFilePath() const
{
    return raw_file_entry;
}

string
ImageFile::createKey() const
{
    return logicalFilePath();
}

vector<unsigned char> *
ImageFile::readData() const
{
    vector<unsigned char> *d;
    switch (fileType())
    {
        case TYPE_RAW:
            d = readImageData();
            break;
        case TYPE_ARCHIVE:
            d = readArchiveData();
            break;
        default:
            return nullptr;
    }
    d->shrink_to_fit();
    return d;
}

bool
ImageFile::isReadableImageFile(const string &path)
{
    int len = sizeof(readable_image) / 
        sizeof(readable_image[0]);
    string ext = string(fl_filename_ext(path.c_str()));
    if (ext.empty()) return false;
    ext.erase(0, 1); // erase a period
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    for (int i = 0; i < len; ++i)
    {
        if (ext == readable_image[i]) return true;
    }
    return false;
}

bool
ImageFile::isReadableArchiveFile(const string &path)
{
    int len = sizeof(readable_archive) / 
        sizeof(readable_archive[0]);
    string ext = string(fl_filename_ext(path.c_str()));
    if (ext.empty()) return false;
    ext.erase(0, 1); //erase a period
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    for (int i = 0; i < len; ++i)
    {
        if (ext == readable_archive[i]) return true;
    }
    return false;
}

const string &
ImageFile::readableFormatExt()
{
    static string extlist;
    if (extlist.empty())
    {
        int i;

        int len_i = sizeof(readable_image) / 
            sizeof(readable_image[0]);
        extlist = "Images\t*.{";
        for (i = 0; i < len_i-1; ++i)
        {
            extlist += readable_image[i];
            extlist += ",";
        }
        extlist += readable_image[i] + "}\n";

        int len_a = sizeof(readable_archive) / 
            sizeof(readable_archive[0]);
        extlist += "Archives\t*.{";
        for (i = 0; i < len_a-1; ++i)
        {
            extlist += readable_archive[i];
            extlist += ",";
        }
        extlist += readable_archive[i] + "}";
    }
    return extlist;
}

vector<ImageFile*>
ImageFile::openArchive(const string &path)
{
    vector<ImageFile*> files;
    if (!isReadableArchiveFile(path)) return files;

    struct archive *a;

    a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);
    int r = archive_read_open_filename(a, path.c_str(), 512*1024);
    if (r != ARCHIVE_OK)
    {
        cerr << archive_error_string(a) << endl;
        archive_read_free(a);
        return files;
    }

    struct archive_entry *ae;
    while (archive_read_next_header(a, &ae) == ARCHIVE_OK)
    {
        const char *raw_entry = archive_entry_pathname(ae);
        string entry_name(raw_entry);
        if (ImageFile::isReadableImageFile(entry_name))
        {
            archive_read_data_skip(a);
            vector<char> re(raw_entry, raw_entry+entry_name.size());
            files.push_back(new ImageFile(path, re));
        }
    }

    r = archive_read_free(a);
    if (r != ARCHIVE_OK)
    {
        cerr << archive_error_string(a) << endl;
    }
    return files;
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

        rfile.seekg(0, ios::beg);
        rfile.clear();

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
    const vector<char> &s_entry = rawFilePath();
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

