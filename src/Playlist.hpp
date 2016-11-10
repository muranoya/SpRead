#ifndef PLAYLIST_HPP
#define PLAYLIST_HPP

#include <FL/Fl_Window.H>
#include <FL/Fl_Multi_Browser.H>
#include <functional>
#include "ImageFile.hpp"
#include "BasicImage.hpp"
#include "Uncopyable.hpp"
#include "OpenDialog.hpp"
#include "SpRead.hpp"

class Playlist : public Fl_Multi_Browser, private Uncopyable
{
public:
    typedef std::function<void(BasicImage*, BasicImage*)>
        ChangeImagesCB;
    typedef std::function<void(void)>
        ChangePlaylistStatusCB;

    explicit Playlist(int x, int y, int w, int h,
            const Fl_Window *parent = 0);
    ~Playlist();

    void openFiles(const StringVec &paths);

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
    const Fl_Window *parent_window;
    int opendirlevel;
    int img_index;
    int img_num;

    /* context menu */
    static void menu_show(Fl_Widget *w, void *arg);
    static void menu_remove(Fl_Widget *w, void *arg);
    static void menu_clear(Fl_Widget *w, void *arg);
    /* context menu */

    void showSelectedItem();
    void changeStatus(int new_idx, int new_num);
    int nextIndex(int idx, int c) const;
    bool isValidIndex(int i) const;
    bool isCurrentIndex(int i) const;

    THREAD_SAFE_FUNC static int openFilesAndDirs(
            const StringVec &paths, int level, Playlist *pl);
    THREAD_SAFE_FUNC static int openFilesAndDirs_File(
            const std::string &path, Playlist *pl);

    void showImages();
    BasicImage *loadData(const ImageItem &f);
};

#endif
