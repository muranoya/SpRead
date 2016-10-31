#include <FL/Fl_Menu_Item.H>
#include <FL/filename.H>
#include <algorithm>
#include "Playlist.hpp"

#include "for_windows_env.hpp"

using namespace std;

Playlist::Playlist(int x, int y, int w, int h)
    : Fl_Multi_Browser(x, y, w, h, "Playlist")
    , changeImages(nullptr)
    , changePlaylistStatus(nullptr)
    , cb_arg(nullptr)
    , opendirlevel(99)
    , img_index(-1)
    , img_num(0)
    , prft()
{
    box(FL_BORDER_BOX);
}

Playlist::~Playlist()
{
}

void
Playlist::openFiles(const vector<string> &paths)
{
    bool req_refresh = (count() < 2);
    clearPlaylist();
    int c = openFilesAndDirs(paths, getOpenDirLevel());
    if (req_refresh && !empty())
    {
        changeStatus(0, 0);
        showImages();
    }
    if (c > 0)
    {
        if (changePlaylistStatus) changePlaylistStatus(cb_arg);
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
        if (changePlaylistStatus) changePlaylistStatus(cb_arg);
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
        if (changePlaylistStatus) changePlaylistStatus(cb_arg);
    }
}

void
Playlist::changeNumOfImages(int n)
{
    bool c = (img_num != n);
    changeStatus(img_index, n);
    if (c)
    {
        if (changePlaylistStatus) changePlaylistStatus(cb_arg);
    }
}

void
Playlist::clearPlaylist()
{
    if (empty()) return;

    for (int i = count(); i > 0; --i)
    {
        ImageFile *f = static_cast<ImageFile*>(data(i));
        delete f;
    }
    clear();

    changeStatus(-1, 0);
    showImages();
    if (changePlaylistStatus) changePlaylistStatus(cb_arg);
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

void
Playlist::setCacheSize(int n)
{
    prft.setCacheSize(n);
}

int
Playlist::getCacheSize() const
{
    return prft.getCacheSize();
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
        ImageFile *d = static_cast<ImageFile*>(data(ti+1));
        return d->logicalFileName();
    }
    return string();
}

void
Playlist::setCallbackUserData(void *arg)
{
    cb_arg = arg;
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

        ImageFile *f = static_cast<ImageFile*>(pl->data(i));
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

    if (pl->changePlaylistStatus) pl->changePlaylistStatus(pl->cb_arg);
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
    if (changePlaylistStatus) changePlaylistStatus(cb_arg);
}

void
Playlist::changeStatus(int new_idx, int new_num)
{
    bool c = (new_idx != img_index || new_num != img_num);

    if (img_index >= 0 && img_num > 0)
    {
        for (int i = 0; i < img_num; ++i)
        {
            if (isValidIndex(img_index+i))
            {
                ImageFile *f = static_cast<ImageFile*>(data(img_index+1+i));
                text(img_index+1+i, f->logicalFileName().c_str());
            }
        }
    }

    img_index = new_idx;
    img_num = new_num;

    if (img_index >= 0 && img_num > 0)
    {
        for (int i = 0; i < img_num; ++i)
        {
            if (isValidIndex(img_index+i))
            {
                ImageFile *f = static_cast<ImageFile*>(data(img_index+1+i));
                string str = "@B10 " + f->logicalFileName();
                text(img_index+1+i, str.c_str());
            }
        }
    }

    if (c)
    {
        redraw();

        vector<ImageFile> list;
        int num = std::min(count(), prft.getCacheSize());
        if (num > 0)
        {
            list.push_back(*static_cast<ImageFile*>(data(img_index+1)));
        }
        for (int i = 1, n = 1; n < num; ++i)
        {
            ImageFile *f;
            f = static_cast<ImageFile*>(data(nextIndex(img_index, i)+1));
            list.push_back(*f); n++;
            if (n >= num) break;
            f = static_cast<ImageFile*>(data(nextIndex(img_index, -i)+1));
            list.push_back(*f); n++;
        }
        reverse(list.begin(), list.end());
        prft.putRequest(list);
    }
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

int
Playlist::openFilesAndDirs(const vector<string> &paths, int level)
{
    int  c = 0;
    if (paths.empty()) return c;

    for (auto iter = paths.cbegin();
            iter != paths.cend(); ++iter)
    {
        if (!fl_filename_isdir(iter->c_str()))
        {
            vector<ImageFile*> lists;
            ImageFile::open(*iter, lists);
            for (auto iter = lists.begin();
                    iter != lists.end(); ++iter)
            {
                add((*iter)->logicalFileName().c_str(), *iter);
            }
            c += lists.size();
        }
        else if (level > 0)
        {
            dirent **dirs;
            int ret = fl_filename_list(iter->c_str(), &dirs, fl_alphasort);
            if (ret > 0)
            {
                string basedir = *iter + "/";
                vector<string> files;
                for (int i = 2; i < ret; ++i)
                {
                    files.push_back(basedir + dirs[i]->d_name);
                }
                c += openFilesAndDirs(files, level-1);
            }
            fl_filename_free_list(&dirs, ret);
        }
    }
    return c;
}

void
Playlist::showImages()
{
    if (!changeImages) return;

    int n = std::min(count(), 2);
    if (n == 2)
    {
        ImageFile *f1 = static_cast<ImageFile*>(data(currentIndex(0)+1));
        ImageFile *f2 = static_cast<ImageFile*>(data(currentIndex(1)+1));
        changeImages(cb_arg, loadData(*f1), loadData(*f2));
    }
    else if (n == 1)
    {
        ImageFile *f1 = static_cast<ImageFile*>(data(currentIndex(0)+1));
        changeImages(cb_arg, loadData(*f1), nullptr);
    }
    else
    {
        changeImages(cb_arg, nullptr, nullptr);
    }
}

BasicImage *
Playlist::loadData(const ImageFile &f)
{
    BasicImage *img = prft.get(f.createKey());
    if (img)
    {
        fprintf(stderr, "cache hit\n");
    }
    else
    {
        fprintf(stderr, "cache miss\n");
        img = f.image();
    }
    return img;
}

