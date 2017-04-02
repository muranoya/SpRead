#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/filename.H>
#include <algorithm>
#include <cstring>
#include <cmath>
#include "Viewer.hpp"
#include "image.hpp"

#include "for_windows_env.hpp"

using namespace std;

Viewer::Viewer(int x, int y, int w, int h)
    : Fl_Widget(x, y, w, h)
    , nextImgReq()
    , prevImgReq()
    , changeNumOfImages()
    , openImageFiles()
    , changeViewerStatus()
    , based_imgs()
    , scaled_imgs()
    , img_num(0)
    , scale_mode(Bilinear)
    , view_mode(FittingWindow)
    , scale_factor(1.0)
    , spread_view(false)
    , rbind_view(false)
    , autospread(false)
    , fp_mode(MouseButton)
    , monomode(false)
    , is_drag_img(false)
    , click_pos()
    , click2_pos()
    , move_pos()
    , img_pos()
    , drag_detect_time(0.2)
{
    based_imgs[0] = nullptr;
    based_imgs[1] = nullptr;
    scaled_imgs[0] = nullptr;
    scaled_imgs[1] = nullptr;
}

Viewer::~Viewer()
{
    delete scaled_imgs[0];
    delete scaled_imgs[1];
}

void
Viewer::showImages(const shared_ptr<BasicImage> &img_l,
        const shared_ptr<BasicImage> &img_r)
{
    if (img_l.get() && monomode) img_l->convertToMono();
    based_imgs[0] = img_l;
    if (img_r.get() && monomode) img_r->convertToMono();
    based_imgs[1] = img_r;
    
    img_pos = Point(0, 0);
    if (getViewMode() == ActualSize ||
            getViewMode() == CustomScale)
    {
        int cw, ch;
        double s;
        if (calc_size(based_imgs[0], based_imgs[1],
                &cw, &ch, nullptr, &s))
        {
            Point pos((w() - cw*s)/2, (h() - ch*s)/2);
            img_pos = pos;
        }
    }
    
    rescaling();
}

void
Viewer::setScalingMode(ScalingMode mode)
{
    bool c = (mode != scale_mode);
    scale_mode = mode;
    if (c) rescaling();
}

Viewer::ScalingMode
Viewer::getScalingMode() const
{
    return scale_mode;
}

void
Viewer::setViewMode(Viewer::ViewMode mode, double factor)
{
    bool c = (mode != view_mode || factor != scale_factor);
    if (0.01 <= factor && factor <= 10)
    {
        view_mode = mode;
        scale_factor = factor;
        if (c) rescaling();
    }
}

void
Viewer::setViewMode(Viewer::ViewMode mode)
{
    setViewMode(mode, scale_factor);
}

double
Viewer::getCustomScaleFactor() const
{
    return scale_factor;
}

Viewer::ViewMode
Viewer::getViewMode() const
{
    return view_mode;
}

void
Viewer::setSpreadView(bool spread)
{
    bool c = (spread_view != spread);
    spread_view = spread;
    if (c) rescaling();
}

bool
Viewer::getSpreadView() const
{
    return spread_view;
}

void
Viewer::setRightbindingView(bool rbind)
{
    bool c = (rbind_view != rbind);
    rbind_view = rbind;
    if (c) rescaling();
}

bool
Viewer::getRightbindingView() const
{
    return rbind_view;
}

void
Viewer::setAutoAdjustSpread(bool aas)
{
    bool c = (autospread != aas);
    autospread = aas;
    if (c) rescaling();
}

bool
Viewer::getAutoAdjustSpread() const
{
    return autospread;
}

void
Viewer::setFeedPageMode(Viewer::FeedPageMode mode)
{
    fp_mode = mode;
}

Viewer::FeedPageMode
Viewer::getFeedPageMode() const
{
    return fp_mode;
}

void
Viewer::setMonochromeMode(bool mode)
{
    monomode = mode;
}

bool
Viewer::getMonochromeMode() const
{
    return monomode;
}

void
Viewer::setNextImageCB(NextImageReqCB cb)
{
    nextImgReq = cb;
}

void
Viewer::setPrevImageCB(PrevImageReqCB cb)
{
    prevImgReq = cb;
}

void
Viewer::setChangeNumOfImagesCB(ChangeNumOfImagesCB cb)
{
    changeNumOfImages = cb;
}

void
Viewer::setOpenImageFilesCB(OpenImageFilesCB cb)
{
    openImageFiles = cb;
}

void
Viewer::setChangeViewerStatusCB(ChangeViewerStatusCB cb)
{
    changeViewerStatus = cb;
}

void
Viewer::draw()
{
    fl_rectf(x(), y(), w(), h(), FL_LIGHT2);

    if (img_num == 0) return;

    BasicImage *imgs[2] = {scaled_imgs[0], scaled_imgs[1]};
    if (img_num == 2 && getRightbindingView())
    {
        BasicImage *img_p = imgs[0];
        imgs[0] = imgs[1];
        imgs[1] = img_p;
    }

    int cimg_w = imgs[0]->width();
    int cimg_h = imgs[0]->height();
    if (img_num == 2)
    {
        cimg_w += imgs[1]->width();
        cimg_h = std::max(cimg_h, imgs[1]->height());
    }

    fl_push_clip(x(), y(), w(), h());
    switch (getViewMode())
    {
        case FittingWindow:
        {
            Point pos((w() - cimg_w)/2 + x(),
                    (h() - imgs[0]->height())/2 + y());
            drawImageToWidget(pos, *imgs[0]);
            if (img_num == 2)
            {
                pos = Point(pos.x() + imgs[0]->width(),
                        (h() - imgs[1]->height())/2 + y());
                drawImageToWidget(pos, *imgs[1]);
            }
        }
        break;
        case FittingWidth:
        {
            img_pos += move_pos - click2_pos;
            click2_pos = move_pos;
            Point pos((w() - cimg_w)/2 + x(),
                    img_pos.y() + (h() - imgs[0]->height())/2 + y());
            drawImageToWidget(pos, *imgs[0]);
            if (img_num == 2)
            {
                pos = Point(pos.x() + imgs[0]->width(),
                        img_pos.y() + (h() - imgs[1]->height())/2 + y());
                drawImageToWidget(pos, *imgs[1]);
            }
        }
        break;
        case ActualSize:
        case CustomScale:
        {
            img_pos += move_pos - click2_pos;
            click2_pos = move_pos;
            Point pos(img_pos.x() + x(),
                    img_pos.y() + (cimg_h - imgs[0]->height())/2 + y());
            drawImageToWidget(pos, *imgs[0]);
            if (img_num == 2)
            {
                pos = Point(pos.x() + imgs[0]->width(),
                        img_pos.y() + (cimg_h - imgs[1]->height())/2 + y());
                drawImageToWidget(pos, *imgs[1]);
            }
        }
        break;
    }
    fl_pop_clip();
}

void
Viewer::resize(int x, int y, int w, int h)
{
    Fl_Widget::resize(x, y, w, h);
    rescaling();
}

int
Viewer::handle(int event)
{
    switch (event)
    {
        case FL_PUSH:
            return mousePressEvent();
        case FL_DRAG:
            return mouseMoveEvent();
        case FL_RELEASE:
            return mouseReleaseEvent();
        case FL_KEYDOWN:
            return keyPressEvent();
        case FL_FOCUS:
        case FL_UNFOCUS:
            return 1;
        case FL_DND_ENTER:
        case FL_DND_LEAVE:
        case FL_DND_DRAG:
        case FL_DND_RELEASE:
            return 1;
        case FL_PASTE:
            {
                StringVec paths;
                string str = Fl::event_text();
                splitWithNewLine(str, paths);
                if (openImageFiles) openImageFiles(paths);
            }
            return 1;
    }
    return Fl_Widget::handle(event);
}

void
Viewer::drag_check(void *arg)
{
    Viewer *v = static_cast<Viewer*>(arg);
    v->is_drag_img = true;
}

int
Viewer::mousePressEvent()
{
    is_drag_img = false;
    switch (getFeedPageMode())
    {
        case MouseButton:
            if (Fl::event_button() == FL_RIGHT_MOUSE)
            {
                if (prevImgReq) prevImgReq();
            }
            else if (Fl::event_button() == FL_LEFT_MOUSE)
            {
                if (getViewMode() == FittingWindow)
                {
                    if (nextImgReq) nextImgReq();
                }
                else
                {
                    move_pos = click_pos = click2_pos =
                        Point(Fl::event_x(), Fl::event_y());
                    Fl::add_timeout(drag_detect_time, drag_check, this);
                }
            }
            return 1;
        case MouseClickPosition:
            if (Fl::event_button() == FL_LEFT_MOUSE)
            {
                if (getViewMode() == FittingWindow)
                {
                    if (Fl::event_x()-x() < w()/2)
                    {
                        if (prevImgReq) prevImgReq();
                    }
                    else
                    {
                        if (nextImgReq) nextImgReq();
                    }
                }
                else
                {
                    move_pos = click_pos = click2_pos =
                        Point(Fl::event_x(), Fl::event_y());
                    Fl::add_timeout(drag_detect_time, drag_check, this);
                }
                return 1;
            }
            break;
    }
    return 0;
}

int
Viewer::mouseMoveEvent()
{
    if (getViewMode() != FittingWindow)
    {
        move_pos = Point(Fl::event_x(), Fl::event_y());
        redraw();
        return 1;
    }
    return 0;
}

int
Viewer::mouseReleaseEvent()
{
    if (getViewMode() != FittingWindow &&
            Fl::event_button() == FL_LEFT_MOUSE && !is_drag_img)
    {
        switch (getFeedPageMode())
        {
            case MouseClickPosition:
                if (Fl::event_x()-x() < w()/2)
                {
                    if (prevImgReq) prevImgReq();
                }
                else
                {
                    if (nextImgReq) nextImgReq();
                }
                break;
            case MouseButton:
                if (nextImgReq) nextImgReq();
                break;
        }
        return 1;
    }
    return 0;
}

int
Viewer::keyPressEvent()
{
    if (Fl::event_key() == FL_Right)
    {
        if (nextImgReq) nextImgReq();
        return 1;
    }
    if (Fl::event_key() == FL_Left)
    {
        if (prevImgReq) prevImgReq();
        return 1;
    }
    return 0;
}

void
Viewer::splitWithNewLine(const string &str,
        StringVec &l)
{
    static char buf[FL_PATH_MAX];
    size_t p, i;
    for (i = p = 0; i < str.length(); ++i)
    {
        switch (str[i])
        {
            case '\r':
                if (str[i+1] == '\n') i++;
                // fall-through
            case '\n':
                buf[p] = '\0';
                fl_decode_uri(buf);
#ifdef WIN32
                l.push_back(buf);
#else
                if (strncmp("file://", buf, sizeof("file://")-1) == 0)
                {
                    l.push_back(buf+sizeof("file://")-1);
                }
#endif
                p = 0;
                break;
            default:
                buf[p++] = str[i];
        }
    }
#ifdef WIN32
    buf[p] = '\0';
    fl_decode_uri(buf);
    l.push_back(buf);
#endif
}

bool
Viewer::calc_size(const shared_ptr<BasicImage> &img1,
        const shared_ptr<BasicImage> &img2,
        int *cimg_w, int *cimg_h, int *img_num, double *scale) const
{
    int cw, ch, n;

    if (getSpreadView() && img1.get() && img2.get())
    {
        n = 2;
        cw = img1->width() + img2->width();
        ch = std::max(img1->height(), img2->height());
        if (getAutoAdjustSpread())
        {
            const double v_wh = w() / static_cast<double>(h());
            const int img1_w = img1->width();
            const int img1_h = img2->height();
            const double img1_wh = img1_w / static_cast<double>(img1_h);
            const double img2_wh = cw / static_cast<double>(ch);
            if (std::fabs(v_wh - img1_wh) < std::fabs(v_wh - img2_wh))
            {
                n = 1;
                cw = img1_w;
                ch = img1_h;
            }
        }
    }
    else if (img1.get())
    {
        n = 1;
        cw = img1->width();
        ch = img1->height();
    }
    else
    {
        if (img_num) *img_num = 0;
        if (cimg_w)  *cimg_w  = 0;
        if (cimg_h)  *cimg_h  = 0;
        return false;
    }

    double s = 1.0;
    if (getViewMode() == CustomScale)
    {
        s = scale_factor;
    }
    else if (getViewMode() == ActualSize)
    {
        s = 1.0;
    }
    else
    {
        double ws = 1.0;
        double hs = 1.0;
        if (w() < cw)
        {
            ws = w() / static_cast<double>(cw);
        }

        if (h() < ch)
        {
            hs = h() / static_cast<double>(ch);
        }

        if (getViewMode() == FittingWindow)
        {
            s = ws > hs ? hs : ws;
        }
        else if (getViewMode() == FittingWidth)
        {
            s = ws;
        }
    }

    if (img_num) *img_num = n;
    if (cimg_w)  *cimg_w  = cw;
    if (cimg_h)  *cimg_h  = ch;
    if (scale)   *scale   = s;
    return true;
}

void
Viewer::rescaling()
{
    const double oldfactor = scale_factor;
    int old_imgnum = img_num;

    delete scaled_imgs[0];
    delete scaled_imgs[1];
    scaled_imgs[0] = nullptr;
    scaled_imgs[1] = nullptr;

    if (!calc_size(based_imgs[0], based_imgs[1],
            nullptr, nullptr, &img_num, &scale_factor))
    {
        redraw();
        return;
    }

    if (scale_factor == 1.0)
    {
        scaled_imgs[0] = new BasicImage(*based_imgs[0]);
        if (img_num == 2) scaled_imgs[1] = new BasicImage(*based_imgs[1]);
    }
    else
    {
        BasicImage *(*f[])(const BasicImage &, double) = {nn, bl, bc};
        for (int i = 0; i < img_num; ++i)
        {
            scaled_imgs[i] = f[scale_mode](*based_imgs[i], scale_factor);
        }
    }

    if (old_imgnum != img_num)
    {
        if (changeNumOfImages) changeNumOfImages(img_num);
    }

    if (oldfactor != scale_factor)
    {
        if (changeViewerStatus) changeViewerStatus();
    }

    redraw();
}

void
Viewer::drawImageToWidget(const Point &pos, const BasicImage &img)
{
    fl_draw_image(img.bits(),
            pos.x(), pos.y(),
            img.width(), img.height(),
            img.depth());
}

