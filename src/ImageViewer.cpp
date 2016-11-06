#include "ImageViewer.hpp"

using namespace std;

ImageViewer::ImageViewer(int x, int y, int w, int h)
    : Fl_Group(x, y, w, h)
    , changeStatus()
    , viewer(nullptr)
    , playlist(nullptr)
    , pl_visible(true)
{
    begin();
    playlist = new Playlist(x, y, 150, h);
    viewer = new Viewer(x+150, y, w-150, h);
    resizable(*viewer);
    end();

    viewer->setNextImageCB(
            [this](){
            playlist->nextImage();
            });
    viewer->setPrevImageCB(
            [this](){
            playlist->prevImage();
            });
    viewer->setChangeNumOfImagesCB(
            [this](int n) {
            playlist->changeNumOfImages(n);
            });
    viewer->setOpenImageFilesCB(
            [this](const vector<string> &paths) {
            playlist->openFiles(paths);
            });
    viewer->setChangeViewerStatusCB(
            [this]() {
            if (changeStatus) changeStatus();
            });

    playlist->setChangeImagesCB(
            [this](BasicImage *l, BasicImage *r) {
            viewer->showImages(l, r);
            });
    playlist->setChangePlaylistStatusCB(
            [this]() {
            if (changeStatus) changeStatus();
            });
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
    if (v && !pl_visible)
    {
        add(playlist);
        playlist->resize(x(), y(), 150, h());
        viewer->resize(x()+150, y(), w()-150, h());
        playlist->redraw();
        pl_visible = true;
    }
    else if (!v && pl_visible)
    {
        remove(playlist);
        viewer->resize(x(), y(), w(), h());
        pl_visible = false;
    }
}

bool
ImageViewer::isPlaylistVisible() const
{
    return pl_visible;
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
ImageViewer::setChangeStatusCB(ChangeStatusCB cb)
{
    changeStatus = cb;
}

