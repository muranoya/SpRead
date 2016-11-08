#include <iostream>
#include <algorithm>
#ifdef SUPPORT_PDF
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-page-renderer.h>
#include <poppler/cpp/poppler-image.h>
#endif
#include "PdfFile.hpp"

using namespace std;

static const vector<string> exts = {
    "pdf",
};

PdfFile::PdfFile(const string &path, const RawData &data)
    : file_path(path)
    , data(data)
{
}

PdfFile::~PdfFile()
{
}

const string &
PdfFile::path() const
{
    return file_path;
}

BasicImage *
PdfFile::loadImage(int index) const
{
#ifdef SUPPORT_PDF
    poppler::document *doc;
    doc = poppler::document::load_from_raw_data(
            reinterpret_cast<const char*>(data.data()), data.size());
    if (!doc || doc->is_encrypted() || doc->is_locked())
    {
        cerr << "Cannot load " << file_path << endl;
        delete doc;
        return nullptr;
    }

    poppler::page *p = doc->create_page(index);
    if (!p)
    {
        cerr << "Cannot load a page of PDF : " << index << endl;
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
            cerr << "Unknown image depth of PDF on page " << index << endl;
            delete doc;
            delete p;
            return nullptr;
    }

    BasicImage *bimg = new BasicImage(img.width(), img.height(), d,
            reinterpret_cast<const uchar*>(img.const_data()));
    delete doc;
    delete p;
    return bimg;
#else
    return nullptr;
#endif
}

bool
PdfFile::isOpenable(const string &ext)
{
#ifdef SUPPORT_PDF
     return any_of(exts.cbegin(), exts.cend(),
             [&ext](const string &x) { return x == ext; });
#else
     return false;
#endif
}

bool
PdfFile::open(const string &path, const RawData &data, vector<ImageItem*> &items)
{
#ifdef SUPPORT_PDF
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
    shared_ptr<ImageFile> f(new PdfFile(path, data));
    for (int i = 0; i < page_num; ++i)
    {
        items.push_back(new ImageItem(f, i, to_string(i)));
    }
    return true;
#else
    return false;
#endif
}

const vector<string> &
PdfFile::extList()
{
    return exts;
}

