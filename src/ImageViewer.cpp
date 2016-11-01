#include "ImageViewer.hpp"

using namespace std;

ImageViewer::ImageViewer(int x, int y, int w, int h)
    : Fl_Group(x, y, w, h)
    , viewer(nullptr)
    , playlist(nullptr)
    , cb_arg(nullptr)
{
    begin();
    playlist = new Playlist(x, y, 150, h);
    viewer = new Viewer(x+150, y, w-150, h);
    resizable(*viewer);
    end();

    viewer->setCallbackUserData(this);
    viewer->setNextImageCB(viewer_NextImageReq);
    viewer->setPrevImageCB(viewer_PrevImageReq);
    viewer->setChangeNumOfImagesCB(viewer_ChangeNumOfImages);
    viewer->setOpenImageFilesCB(viewer_OpenImageFiles);
    viewer->setChangeViewerStatusCB(viewer_ChangeViewerStatus);

    playlist->setCallbackUserData(this);
    playlist->setChangeImagesCB(playlist_ChangeImages);
    playlist->setChangePlaylistStatusCB(playlist_ChangePlaylistStatus);
}

ImageViewer::~ImageViewer()
{
    delete viewer;
    delete playlist;
}

void
ImageViewer::openFiles(const vector<string> &paths)
{
    playlist->openFiles(paths);
}

void
ImageViewer::clearPlaylist()
{
    playlist->clearPlaylist();
}

void
ImageViewer::playlistVisible(bool v)
{
    if (v)
    {
        add(playlist);
        playlist->resize(x(), y(), 150, h());
        viewer->resize(x()+150, y(), w()-150, h());
        playlist->redraw();
    }
    else
    {
        remove(playlist);
        viewer->resize(x(), y(), w(), h());
    }
}

bool
ImageViewer::isPlaylistVisible() const
{
    return static_cast<Fl_Widget*>(playlist)->visible() != 0;
}

void
ImageViewer::setScalingMode(Viewer::ScalingMode mode)
{
    viewer->setScalingMode(mode);
}

Viewer::ScalingMode
ImageViewer::getScalingMode() const
{
    return viewer->getScalingMode();
}

void
ImageViewer::setViewMode(Viewer::ViewMode mode, double factor)
{
    viewer->setViewMode(mode, factor);
}

void
ImageViewer::setViewMode(Viewer::ViewMode mode)
{
    viewer->setViewMode(mode);
}

double
ImageViewer::getCustomScaleFactor() const
{
    return viewer->getCustomScaleFactor();
}

Viewer::ViewMode
ImageViewer::getViewMode() const
{
    return viewer->getViewMode();
}

void
ImageViewer::setSpreadView(bool spread)
{
    viewer->setSpreadView(spread);
}

bool
ImageViewer::getSpreadView() const
{
    return viewer->getSpreadView();
}

void
ImageViewer::setRightbindingView(bool rbind)
{
    viewer->setRightbindingView(rbind);
}

bool
ImageViewer::getRightbindingView() const
{
    return viewer->getRightbindingView();
}

void
ImageViewer::setAutoAdjustSpread(bool aas)
{
    viewer->setAutoAdjustSpread(aas);
}

bool
ImageViewer::getAutoAdjustSpread() const
{
    return viewer->getAutoAdjustSpread();
}

void
ImageViewer::setFeedPageMode(Viewer::FeedPageMode mode)
{
    viewer->setFeedPageMode(mode);
}

Viewer::FeedPageMode
ImageViewer::getFeedPageMode() const
{
    return viewer->getFeedPageMode();
}

void
ImageViewer::setOpenDirLevel(int n)
{
    playlist->setOpenDirLevel(n);
}

int
ImageViewer::getOpenDirLevel() const
{
    return playlist->getOpenDirLevel();
}

int
ImageViewer::countShowImages() const
{
    return playlist->countShowImages();
}

int
ImageViewer::count() const
{
    return playlist->count();
}

bool
ImageViewer::empty() const
{
    return playlist->empty();
}

int
ImageViewer::currentIndex(int i) const
{
    return playlist->currentIndex(i);
}

string
ImageViewer::currentFileName(int i) const
{
    return playlist->currentFileName(i);
}

void
ImageViewer::setChangeStatusCB(ChangeStatusCB cb, void *arg)
{
    changeStatus = cb;
    cb_arg = arg;
}

void
ImageViewer::viewer_NextImageReq(void *arg)
{
    ImageViewer *iv = static_cast<ImageViewer*>(arg);
    iv->playlist->nextImage();
}

void
ImageViewer::viewer_PrevImageReq(void *arg)
{
    ImageViewer *iv = static_cast<ImageViewer*>(arg);
    iv->playlist->prevImage();
}

void
ImageViewer::viewer_ChangeNumOfImages(void *arg, int n)
{
    ImageViewer *iv = static_cast<ImageViewer*>(arg);
    iv->playlist->changeNumOfImages(n);
}

void
ImageViewer::viewer_OpenImageFiles(void *arg,
            const vector<string> &paths)
{
    ImageViewer *iv = static_cast<ImageViewer*>(arg);
    iv->playlist->openFiles(paths);
}

void
ImageViewer::viewer_ChangeViewerStatus(void *arg)
{
    ImageViewer *iv = static_cast<ImageViewer*>(arg);
    if (iv->changeStatus) iv->changeStatus(iv->cb_arg);
}

void
ImageViewer::playlist_ChangeImages(void *arg,
            BasicImage *img_l, BasicImage *img_r)
{
    ImageViewer *iv = static_cast<ImageViewer*>(arg);
    iv->viewer->showImages(img_l, img_r);
}

void
ImageViewer::playlist_ChangePlaylistStatus(void *arg)
{
    ImageViewer *iv = static_cast<ImageViewer*>(arg);
    if (iv->changeStatus) iv->changeStatus(iv->cb_arg);
}

