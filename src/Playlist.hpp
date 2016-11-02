#ifndef PLAYLIST_HPP
#define PLAYLIST_HPP

#include <FL/Fl_Multi_Browser.H>
#include <vector>
#include <string>
#include <functional>
#include "ImageFile.hpp"
#include "BasicImage.hpp"

class Playlist : public Fl_Multi_Browser
{
public:
    typedef std::function<void(BasicImage*, BasicImage*)>
        ChangeImagesCB;
    typedef std::function<void(void)>
        ChangePlaylistStatusCB;

    explicit Playlist(int x, int y, int w, int h);
    ~Playlist();

    void openFiles(const std::vector<std::string> &paths);

    void nextImage();
    void prevImage();
    void changeNumOfImages(int n);

    void clearPlaylist();

    void setOpenDirLevel(int n);
    int getOpenDirLevel() const;

    int countShowImages() const;
    int count() const;
    bool empty() const;

    int currentIndex(int i) const;
    std::string currentFileName(int i) const;

    void setChangeImagesCB(ChangeImagesCB cb);
    void setChangePlaylistStatusCB(ChangePlaylistStatusCB cb);

    int handle(int event);

private:
    ChangeImagesCB changeImages;
    ChangePlaylistStatusCB changePlaylistStatus;

    int opendirlevel;
    int img_index;
    int img_num;

    static void menu_show(Fl_Widget *w, void *arg);
    static void menu_remove(Fl_Widget *w, void *arg);
    static void menu_clear(Fl_Widget *w, void *arg);

    void showSelectedItem();
    void changeStatus(int new_idx, int new_num);
    int nextIndex(int idx, int c) const;
    bool isValidIndex(int i) const;
    bool isCurrentIndex(int i) const;

    int openFilesAndDirs(const std::vector<std::string> &paths, int level);

    void showImages();
    BasicImage *loadData(const ImageItem &f);
};

#endif
