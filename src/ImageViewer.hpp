#ifndef IMAGEVIEWER_HPP
#define IMAGEVIEWER_HPP

#include <FL/Fl_Group.H>
#include <functional>
#include "Viewer.hpp"
#include "Playlist.hpp"
#include "BasicImage.hpp"
#include "Uncopyable.hpp"
#include "SpRead.hpp"

class ImageViewer : public Fl_Group, private Uncopyable
{
public:
    typedef std::function<void(void)>
        ChangeStatusCB;

    explicit ImageViewer(int x, int y, int w, int h);
    ~ImageViewer();

    void openFiles(const StringVec &paths);
    void clearPlaylist();

    void playlistVisible(bool v);
    bool isPlaylistVisible() const;

    void setScalingMode(Viewer::ScalingMode mode);
    Viewer::ScalingMode getScalingMode() const;

    void setViewMode(Viewer::ViewMode mode, double factor);
    void setViewMode(Viewer::ViewMode mode);
    double getCustomScaleFactor() const;
    Viewer::ViewMode getViewMode() const;

    void setSpreadView(bool spread);
    bool getSpreadView() const;

    void setRightbindingView(bool rbind);
    bool getRightbindingView() const;

    void setAutoAdjustSpread(bool aas);
    bool getAutoAdjustSpread() const;

    void setFeedPageMode(Viewer::FeedPageMode mode);
    Viewer::FeedPageMode getFeedPageMode() const;

    void setOpenDirLevel(int n);
    int getOpenDirLevel() const;

    int countShowImages() const;
    int count() const;
    bool empty() const;

    int currentIndex(int i) const;
    std::string currentFileName(int i) const;

    void setChangeStatusCB(ChangeStatusCB cb);

private:
    ChangeStatusCB changeStatus;
    Viewer *viewer;
    Playlist *playlist;
    bool pl_visible;
};

#endif // IMAGEVIEWER_HPP
