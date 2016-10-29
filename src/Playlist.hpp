#ifndef PLAYLIST_HPP
#define PLAYLIST_HPP

#include <FL/Fl_Multi_Browser.H>
#include <vector>
#include <string>
#include "ImageFile.hpp"
#include "Prefetcher.hpp"
#include "BasicImage.hpp"

class Playlist : public Fl_Multi_Browser
{
public:
    typedef void (*ChangeImagesCB)(void *arg,
            BasicImage *img_l, BasicImage *img_r);
    typedef void (*ChangePlaylistStatusCB)(void *arg);

    explicit Playlist(int x, int y, int w, int h);
    ~Playlist();

    void openFiles(const std::vector<std::string> &paths);

    void nextImage();
    void prevImage();
    void changeNumOfImages(int n);

    void clearPlaylist();

    void setOpenDirLevel(int n);
    int getOpenDirLevel() const;

    void setCacheSize(int n);
    int getCacheSize() const;

    int countShowImages() const;
    int count() const;
    bool empty() const;

    int currentIndex(int i) const;
    std::string currentFileName(int i) const;

    void setCallbackUserData(void *arg);
    void setChangeImagesCB(ChangeImagesCB cb);
    void setChangePlaylistStatusCB(ChangePlaylistStatusCB cb);

    int handle(int event);

private:
    ChangeImagesCB changeImages;
    ChangePlaylistStatusCB changePlaylistStatus;
    void *cb_arg;

    int opendirlevel;
    int img_index;
    int img_num;
    Prefetcher prft;

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
    BasicImage *loadData(const ImageFile &f);
};

#endif
