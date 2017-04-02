#include <iostream>
#include <algorithm>
#include <vector>
#ifdef SUPPORT_PDF
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-page-renderer.h>
#include <poppler/cpp/poppler-image.h>
#endif
#include "PdfFile.hpp"

using namespace std;

static const StringVec exts = {
#ifdef SUPPORT_PDF
    "pdf",
#endif
};

PdfFile::PdfFile(const string &path, const RawData &data)
    : file_path(path)
    , data(data)
{
#ifdef SUPPORT_PDF
    doc = poppler::document::load_from_raw_data(
            reinterpret_cast<const char*>(this->data.data()),
            this->data.size());
#endif
}

PdfFile::~PdfFile()
{
#ifdef SUPPORT_PDF
    delete doc;
#endif
}

const string &
PdfFile::path() const
{
    return file_path;
}

shared_ptr<BasicImage>
PdfFile::loadImage(int page)
{
    return shared_ptr<BasicImage>(rendering_page(doc, page));
}

THREAD_SAFE_FUNC bool
PdfFile::isOpenable(const string &ext)
{
#ifdef SUPPORT_PDF
     return any_of(exts.cbegin(), exts.cend(),
             [&ext](const string &x) { return x == ext; });
#else
     return false;
#endif
}

THREAD_SAFE_FUNC bool
PdfFile::open_pdf(const string &path, const RawData &data,
        vector<ImageItem*> &items)
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

const StringVec &
PdfFile::extList()
{
    return exts;
}

BasicImage *
PdfFile::rendering_page(poppler::document *doc, int page)
{
#ifdef SUPPORT_PDF
    poppler::page *p = doc->create_page(page);
    if (!p)
    {
        cerr << "Cannot load a page of PDF : " << page << endl;
        delete p;
        return nullptr;
    }
    poppler::page_renderer renderer;
    renderer.set_render_hints(
            poppler::page_renderer::antialiasing |
            poppler::page_renderer::text_antialiasing |
            poppler::page_renderer::text_hinting);
    poppler::image img = renderer.render_page(p, 120.0, 120.0);
    int d;
    switch (img.format())
    {
        case poppler::image::format_mono:   d = 1; break;
        case poppler::image::format_rgb24:  d = 3; break;
        case poppler::image::format_argb32: d = 4; break; 
        default:
            cerr << "Unknown image depth of PDF on page "
                 << page << endl;
            delete p;
            return nullptr;
    }

    // convert the image from 32bit-color to 24bit-color
    if (d == 4)
    {
        uchar *bits3 = reinterpret_cast<uchar*>(img.data());
        const uchar *bits4 = bits3;
        int w = img.width();
        int h = img.height();
        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                uchar r = bits4[0];
                uchar g = bits4[1];
                uchar b = bits4[2];
                bits3[0] = b;
                bits3[1] = g;
                bits3[2] = r;
                bits3 += 3;
                bits4 += 4;
            }
        }
        d = 3;
    }

    BasicImage *bimg = new BasicImage(img.width(), img.height(), d,
            reinterpret_cast<const uchar*>(img.const_data()));
    delete p;
    return bimg;
#else
    return nullptr;
#endif
}

