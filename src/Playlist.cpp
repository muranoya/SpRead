#include <FL/Fl.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/filename.H>
#include <algorithm>
#include <future>
#include "Playlist.hpp"

#include "for_windows_env.hpp"

using namespace std;

Playlist::Playlist(int x, int y, int w, int h, const Fl_Window *parent)
    : Fl_Multi_Browser(x, y, w, h, "Playlist")
    , changeImages()
    , changePlaylistStatus()
    , parent_window(parent)
    , opendirlevel(99)
    , img_index(-1)
    , img_num(0)
{
    box(FL_BORDER_BOX);
}

Playlist::~Playlist()
{
}

void
Playlist::openFiles(const StringVec &paths)
{
    clearPlaylist();
    OpenDialog dlg(parent_window);
    future<int> f = async(launch::async,
            [&](int n){
            int c = openFilesAndDirs(paths, n, this);
            Fl::lock();
            dlg.asyncDone();
            Fl::unlock();
            return c;
            }, getOpenDirLevel());
    dlg.showDialog();
    int c = f.get();

    if (!empty())
    {
        changeStatus(0, 0);
        showImages();
    }
    if (c > 0)
    {
        if (changePlaylistStatus) changePlaylistStatus();
    }
}

void
Playlist::nextImage()
{
    if (empty()) return;

    int old_index = img_index;

    changeStatus(nextIndex(img_index, countShowImages()),
            countShowImages());
    
    if (old_index != img_index)
    {
        showImages();
        if (changePlaylistStatus) changePlaylistStatus();
    }
}

void
Playlist::prevImage()
{
    if (empty()) return;

    int old_index = img_index;

    changeStatus(nextIndex(img_index, -countShowImages()),
            countShowImages());

    if (old_index != img_index)
    {
        showImages();
        if (changePlaylistStatus) changePlaylistStatus();
    }
}

void
Playlist::changeNumOfImages(int n)
{
    bool c = (img_num != n);
    changeStatus(img_index, n);
    if (c)
    {
        if (changePlaylistStatus) changePlaylistStatus();
    }
}

void
Playlist::clearPlaylist()
{
    if (empty()) return;

    for (int i = count(); i > 0; --i)
    {
        ImageItem *f = static_cast<ImageItem*>(data(i));
        delete f;
    }
    clear();

    changeStatus(-1, 0);
    showImages();
    if (changePlaylistStatus) changePlaylistStatus();
}

void
Playlist::setOpenDirLevel(int n)
{
    opendirlevel = n;
}

int
Playlist::getOpenDirLevel() const
{
    return opendirlevel;
}

int
Playlist::countShowImages() const
{
    return img_num;
}

int
Playlist::count() const
{
    return size();
}

bool
Playlist::empty() const
{
    return count() == 0;
}

int
Playlist::currentIndex(int i) const
{
    if (0 <= i && i < 2)
    {
        return (img_index + i) % count();
    }
    return -1;
}

string
Playlist::currentFileName(int i) const
{
    const int ti = currentIndex(i);
    if (isValidIndex(ti))
    {
        ImageItem *d = static_cast<ImageItem*>(data(ti+1));
        return d->virtualName();
    }
    return string();
}

void
Playlist::setChangeImagesCB(ChangeImagesCB cb)
{
    changeImages = cb;
}

void
Playlist::setChangePlaylistStatusCB(ChangePlaylistStatusCB cb)
{
    changePlaylistStatus = cb;
}

int
Playlist::handle(int event)
{
    switch (event)
    {
        case FL_PUSH:
            if (Fl::event_button() == FL_RIGHT_MOUSE)
            {
                static Fl_Menu_Item menus[] = {
                    {"&Show",   0, menu_show,   this, FL_MENU_DIVIDER},
                    {"&Remove", 0, menu_remove, this},
                    {"&Clear",  0, menu_clear,  this},
                    {0},
                };
                const Fl_Menu_Item *m = menus->popup(Fl::event_x(),
                        Fl::event_y(), 0, 0, 0);
                if (m) m->do_callback(0, this);
               return 1;
            }
            else if (Fl::event_clicks())
            {
                showSelectedItem();
                return 1;
            }
            break;
    }
    return Fl_Multi_Browser::handle(event);
}

void
Playlist::menu_show(Fl_Widget *w, void *arg)
{
    Playlist *pl = static_cast<Playlist*>(arg);
    pl->showSelectedItem();
}

void
Playlist::menu_remove(Fl_Widget *w, void *arg)
{
    Playlist *pl = static_cast<Playlist*>(arg);
    if (pl->empty() || pl->value() <= 0) return;

    bool contain = (pl->selected(pl->img_index+1) ||
            (pl->countShowImages() == 2 && pl->selected(pl->img_index+2)));

    int i;
    int c = 0;
    const int mi = pl->img_index;
    for (i = 1; i <= mi; ++i)
    {
        if (!pl->selected(i)) continue;
        c++;
    }
    
    int len = pl->count();
    for (i = 1; i <= len; )
    {
        if (!pl->selected(i))
        {
            i++;
            continue;
        }

        ImageItem *f = static_cast<ImageItem*>(pl->data(i));
        delete f;
        pl->remove(i);
        len--;
    }

    if (pl->empty())
    {
        pl->changeStatus(-1, 0);
        pl->showImages();
    }
    else
    {
        pl->changeStatus(pl->img_index - c,
                std::min(pl->count(), pl->countShowImages()));
        if (contain) pl->showImages();
    }

    if (pl->changePlaylistStatus) pl->changePlaylistStatus();
}

void
Playlist::menu_clear(Fl_Widget *w, void *arg)
{
    Playlist *pl = static_cast<Playlist*>(arg);
    pl->clearPlaylist();
}

void
Playlist::showSelectedItem()
{
    if (empty() || value() <= 0) return;

    changeStatus(value()-1, img_num);
    showImages();
    if (changePlaylistStatus) changePlaylistStatus();
}

void
Playlist::changeStatus(int new_idx, int new_num)
{
    bool c = (new_idx != img_index || new_num != img_num);

    if (img_index >= 0 && img_num > 0 && count() > 0)
    {
        for (int i = 0; i < img_num; ++i)
        {
            int idx = (img_index+i) % count();
            if (isValidIndex(idx))
            {
                ImageItem *f = static_cast<ImageItem*>(data(idx+1));
                text(idx+1, f->virtualName().c_str());
            }
        }
    }

    img_index = new_idx;
    img_num = new_num;

    if (img_index >= 0 && img_num > 0 && count() > 0)
    {
        for (int i = 0; i < img_num; ++i)
        {
            int idx = (img_index+i) % count();
            if (isValidIndex(idx))
            {
                ImageItem *f = static_cast<ImageItem*>(data(idx+1));
                string str = "@B10 " + f->virtualName();
                text(idx+1, str.c_str());
            }
        }
    }

    if (c) redraw();
}

int
Playlist::nextIndex(int idx, int c) const
{
    int i = (idx + c) % count();
    if (i < 0) i += count();
    return i;
}

bool
Playlist::isValidIndex(int i) const
{
    return (0 <= i && i < count());
}

bool
Playlist::isCurrentIndex(int idx) const
{
    for (int n = 0; n < countShowImages(); ++n)
    {
        if (idx == currentIndex(n)) return true;
    }
    return false;
}

THREAD_SAFE_FUNC int
Playlist::openFilesAndDirs(const StringVec &paths, int level, Playlist *pl)
{
    int  c = 0;
    if (paths.empty()) return c;

    for (auto iter : paths)
    {
        if (!fl_filename_isdir(iter.c_str()))
        {
            c += openFilesAndDirs_File(iter, pl);
        }
        else if (level > 0)
        {
            dirent **dirs;
            int ret = fl_filename_list(iter.c_str(), &dirs, fl_alphasort);
            if (ret > 0)
            {
                string basedir = iter + "/";
                StringVec files;
                for (int i = 2; i < ret; ++i)
                {
                    files.push_back(basedir + dirs[i]->d_name);
                }
                c += openFilesAndDirs(files, level-1, pl);
            }
            fl_filename_free_list(&dirs, ret);
        }
    }
    return c;
}

THREAD_SAFE_FUNC int
Playlist::openFilesAndDirs_File(const string &path, Playlist *pl)
{
    vector<ImageItem*> lists;
    auto rslt = ImageFile::open(path, lists);
    if (rslt == ImageFile::OpenSuccess)
    {
        struct AsyncData
        {
            AsyncData(const vector<ImageItem*> &items, Playlist *pl)
                : items(items), pl(pl) {}
            vector<ImageItem*> items;
            Playlist *pl;
        };
        Fl::awake([](void *arg) {
                AsyncData *ad = static_cast<AsyncData*>(arg);
                for (auto iter : ad->items)
                {
                    ad->pl->add(iter->virtualName().c_str(), iter);
                }
                delete ad;
                }, new AsyncData(lists, pl));
    }
    return lists.size();
}

void
Playlist::showImages()
{
    if (!changeImages) return;

    int n = std::min(count(), 2);
    if (n == 2)
    {
        ImageItem *f1 = static_cast<ImageItem*>(data(currentIndex(0)+1));
        ImageItem *f2 = static_cast<ImageItem*>(data(currentIndex(1)+1));
        changeImages(f1->image(), f2->image());
    }
    else if (n == 1)
    {
        ImageItem *f1 = static_cast<ImageItem*>(data(currentIndex(0)+1));
        changeImages(f1->image(), nullptr);
    }
    else
    {
        changeImages(nullptr, nullptr);
    }
}

