#include <FL/Fl_Native_File_Chooser.H>
#include <cstdlib>
#include "MainWindow.hpp"
#include "ConfigDialog.hpp"
#include "MagDialog.hpp"
#include "Config.hpp"

using namespace std;

MainWindow::MainWindow(int argc, char *argv[],
        int x, int y, int w, int h)
    : Fl_Double_Window(x, y, w, h, Config::SOFTWARE_NAME.c_str())
    , menubar(nullptr)
    , imgviewer(nullptr)
{
    createMenus();

    begin();
    imgviewer = new ImageViewer(0, menubar->h(),
            this->w(), this->h() - menubar->h());
    resizable(imgviewer);
    end();

    applyConfig();

    imgviewer->setChangeStatusCB([this](){ updateWindowTitle(); });

    if (argc >= 2)
    {
        StringVec paths;
        for (int i = 1; i < argc; ++i)
        {
            paths.push_back(argv[i]);
        }
        imgviewer->openFiles(paths);
    }

    updateWindowTitle();
}

MainWindow::~MainWindow()
{
    delete imgviewer;
    delete menubar;
}

void
MainWindow::hide()
{
    storeConfig();
    Config::SaveConfig();
    Fl_Double_Window::hide();
}

void
MainWindow::file_open(Fl_Widget *w, void *arg)
{
    MainWindow *mw = static_cast<MainWindow*>(arg);

    Fl_Native_File_Chooser file_dlg(Fl_Native_File_Chooser::BROWSE_MULTI_FILE);
    file_dlg.title("Select Files");
    file_dlg.filter(ImageFile::readableFormatExtList().c_str());

    if (file_dlg.show() == 0)
    {
        StringVec files;
        for (int i = 0; i < file_dlg.count(); ++i)
        {
            string filename = file_dlg.filename(i);
            files.push_back(filename);
        }
        mw->imgviewer->openFiles(files);
    }
}

void
MainWindow::file_opendir(Fl_Widget *w, void *arg)
{
    MainWindow *mw = static_cast<MainWindow*>(arg);

    Fl_Native_File_Chooser dir_dlg(Fl_Native_File_Chooser::BROWSE_MULTI_DIRECTORY);
    dir_dlg.title("Select Directories");

    if (dir_dlg.show() == 0)
    {
        StringVec dirs;
        for (int i = 0; i < dir_dlg.count(); ++i)
        {
            string dirname = dir_dlg.filename(i);
            dirs.push_back(dirname);
        }
        mw->imgviewer->openFiles(dirs);
    }
}

void
MainWindow::file_config(Fl_Widget *w, void *arg)
{
    MainWindow *mw = static_cast<MainWindow*>(arg);
    bool rslt = ConfigDialog::openConfigDialog(mw);
    if (rslt)
    {
        mw->imgviewer->setOpenDirLevel(Config::view_openlevel);
        mw->imgviewer->setFeedPageMode(
                static_cast<Viewer::FeedPageMode>(Config::view_feedpage));
    }
}

void
MainWindow::file_exit(Fl_Widget *w, void *arg)
{
    MainWindow *mw = static_cast<MainWindow*>(arg);
    mw->hide();
}

void
MainWindow::view_actualsize(Fl_Widget *w, void *arg)
{
    MainWindow *mw = static_cast<MainWindow*>(arg);
    mw->imgviewer->setViewMode(Viewer::ActualSize);
}

void
MainWindow::view_fitwindow(Fl_Widget *w, void *arg)
{
    MainWindow *mw = static_cast<MainWindow*>(arg);
    mw->imgviewer->setViewMode(Viewer::FittingWindow);
}

void
MainWindow::view_fitwidth(Fl_Widget *w, void *arg)
{
    MainWindow *mw = static_cast<MainWindow*>(arg);
    mw->imgviewer->setViewMode(Viewer::FittingWidth);
}

void
MainWindow::view_specmag(Fl_Widget *w, void *arg)
{
    MainWindow *mw = static_cast<MainWindow*>(arg);
    double f;
    bool rslt = MagDialog::getFactor(
            mw->imgviewer->getCustomScaleFactor(),
            f);
    if (rslt)
    {
        mw->imgviewer->setViewMode(Viewer::CustomScale, f);
    }
    else
    {
        Fl_Menu_Item *mitems[] = {
            mw->findMenuItem(view_actualsize),
            mw->findMenuItem(view_fitwindow),
            mw->findMenuItem(view_fitwidth),
            mw->findMenuItem(view_specmag),
        };
        mitems[mw->imgviewer->getViewMode()]->set();
        mitems[Viewer::CustomScale]->clear();
    }
}

void
MainWindow::view_spread(Fl_Widget *w, void *arg)
{
    MainWindow *mw = static_cast<MainWindow*>(arg);
    const Fl_Menu_Item *m = mw->findMenuItem(view_spread);
    mw->imgviewer->setSpreadView(m->value() != 0);
}

void
MainWindow::view_autospread(Fl_Widget *w, void *arg)
{
    MainWindow *mw = static_cast<MainWindow*>(arg);
    const Fl_Menu_Item *m = mw->findMenuItem(view_autospread);
    mw->imgviewer->setAutoAdjustSpread(m->value() != 0);
}

void
MainWindow::view_rightbinding(Fl_Widget *w, void *arg)
{
    MainWindow *mw = static_cast<MainWindow*>(arg);
    const Fl_Menu_Item *m = mw->findMenuItem(view_rightbinding);
    mw->imgviewer->setRightbindingView(m->value() != 0);
}

void
MainWindow::view_nn(Fl_Widget *w, void *arg)
{
    MainWindow *mw = static_cast<MainWindow*>(arg);
    mw->imgviewer->setScalingMode(Viewer::NearestNeighbor);
}

void
MainWindow::view_bl(Fl_Widget *w, void *arg)
{
    MainWindow *mw = static_cast<MainWindow*>(arg);
    mw->imgviewer->setScalingMode(Viewer::Bilinear);
}

void
MainWindow::view_bc(Fl_Widget *w, void *arg)
{
    MainWindow *mw = static_cast<MainWindow*>(arg);
    mw->imgviewer->setScalingMode(Viewer::Bicubic);
}

void
MainWindow::window_playlist(Fl_Widget *w, void *arg)
{
    MainWindow *mw = static_cast<MainWindow*>(arg);
    const Fl_Menu_Item *m = mw->findMenuItem(window_playlist);
    mw->imgviewer->playlistVisible(m->value() != 0);
}

void
MainWindow::updateWindowTitle()
{
    if (imgviewer->empty())
    {
        label(Config::SOFTWARE_NAME.c_str());
        return;
    }

    static string title;
    if (imgviewer->countShowImages() == 1)
    {
        title =
            "[" +
            to_string(imgviewer->currentIndex(0)+1) +
            "/" +
            to_string(imgviewer->count()) +
            "]";
    }
    else
    {
        title =
            "[" +
            to_string(imgviewer->currentIndex(0)+1) +
            "-" +
            to_string(imgviewer->currentIndex(1)+1) +
            "/" +
            to_string(imgviewer->count()) +
            "]";
    }

    if (imgviewer->countShowImages() == 2)
    {
        string title2;
        if (imgviewer->getRightbindingView())
        {
            title2 =
                imgviewer->currentFileName(1) +
                " | " +
                imgviewer->currentFileName(0);
        }
        else
        {
            title2 =
                imgviewer->currentFileName(0) +
                " | " +
                imgviewer->currentFileName(1);
        }
        title += " " + title2;
    }
    else
    {
        title += " " + imgviewer->currentFileName(0);
    }

    char buf[32];
    snprintf(buf, sizeof(buf), " %0.2f%%", imgviewer->getCustomScaleFactor()*100.0);
    title += buf;
    label(title.c_str());
}

Fl_Menu_Item *
MainWindow::findMenuItem(Fl_Callback *callback)
{
    return const_cast<Fl_Menu_Item*>(menubar->find_item(callback));
}

void
MainWindow::createMenus()
{
    static Fl_Menu_Item Main_Menu[] = {
    {"&File", 0, 0, 0, FL_SUBMENU},
      {"&Open",           FL_COMMAND+'o', file_open,         this},
      {"Open &Directory", FL_COMMAND+'i', file_opendir,      this, FL_MENU_DIVIDER},
      {"&Config",                      0, file_config,       this, FL_MENU_DIVIDER},
      {"&Exit",           FL_COMMAND+'q', file_exit,         this},
      {0},
    {"&View", 0, 0, 0, FL_SUBMENU},
      {"&Actual Size",                 0, view_actualsize,   this, FL_MENU_RADIO},
      {"&Fit Window",                  0, view_fitwindow,    this, FL_MENU_RADIO},
      {"Fit &Width",                   0, view_fitwidth,     this, FL_MENU_RADIO},
      {"Specify &Magnification",       0, view_specmag,      this, FL_MENU_RADIO | FL_MENU_DIVIDER},
      {"&Spread",                      0, view_spread,       this, FL_MENU_TOGGLE},
      {"A&uto Spread",                 0, view_autospread,   this, FL_MENU_TOGGLE},
      {"&Right Binding",               0, view_rightbinding, this, FL_MENU_TOGGLE | FL_MENU_DIVIDER},
      {"&Low Qual(Nearest Neighbor)",  0, view_nn,           this, FL_MENU_RADIO},
      {"&Balance Qual(Bilinear)",      0, view_bl,           this, FL_MENU_RADIO},
      {"&High Qual(Bicubic)",          0, view_bc,           this, FL_MENU_RADIO},
      {0},
    {"&Window", 0, 0, 0, FL_SUBMENU},
      {"&Playlist",       FL_COMMAND+'p', window_playlist,   this, FL_MENU_TOGGLE},
      {0},
    {0}};

    begin();
    menubar = new Fl_Menu_Bar(0, 0, w(), 30);
    menubar->menu(Main_Menu);
    menubar->global();
    menubar->box(FL_FLAT_BOX);
    end();
}

void
MainWindow::applyConfig()
{
    resize(Config::mw_pos_x,  Config::mw_pos_y,
           Config::mw_size_w, Config::mw_size_h);
    imgviewer->setScalingMode(
            static_cast<Viewer::ScalingMode>(Config::view_ipix));
    imgviewer->setViewMode(
            static_cast<Viewer::ViewMode>(Config::view_scalem), Config::view_scale);
    imgviewer->setSpreadView(Config::view_spread);
    imgviewer->setRightbindingView(Config::view_rbind);
    imgviewer->setAutoAdjustSpread(Config::view_autospread);
    imgviewer->setFeedPageMode(
            static_cast<Viewer::FeedPageMode>(Config::view_feedpage));
    imgviewer->setOpenDirLevel(Config::view_openlevel);
    imgviewer->playlistVisible(Config::pl_visible);

    switch (imgviewer->getScalingMode())
    {
        case Viewer::NearestNeighbor:
            findMenuItem(view_nn)->set();
            break;
        case Viewer::Bilinear:
            findMenuItem(view_bl)->set();
            break;
        case Viewer::Bicubic:
            findMenuItem(view_bc)->set();
            break;
    }

    if (imgviewer->getSpreadView())       findMenuItem(view_spread)->set();
    if (imgviewer->getAutoAdjustSpread()) findMenuItem(view_autospread)->set();
    if (imgviewer->getRightbindingView()) findMenuItem(view_rightbinding)->set();

    switch (imgviewer->getViewMode())
    {
        case Viewer::ActualSize:
            findMenuItem(view_actualsize)->set();
            break;
        case Viewer::FittingWindow:
            findMenuItem(view_fitwindow)->set();
            break;
        case Viewer::FittingWidth:
            findMenuItem(view_fitwidth)->set();
            break;
        case Viewer::CustomScale:
            findMenuItem(view_specmag)->set();
            break;
    }

    if (imgviewer->isPlaylistVisible()) findMenuItem(window_playlist)->set();
}

void
MainWindow::storeConfig()
{
    Config::mw_size_w = w();
    Config::mw_size_h = h();
    Config::mw_pos_x  = x();
    Config::mw_pos_y  = y();

    Config::view_scalem     = imgviewer->getViewMode();
    Config::view_scale      = imgviewer->getCustomScaleFactor();
    Config::view_ipix       = imgviewer->getScalingMode();
    Config::view_spread     = imgviewer->getSpreadView();
    Config::view_autospread = imgviewer->getAutoAdjustSpread();
    Config::view_rbind      = imgviewer->getRightbindingView();
    Config::view_openlevel  = imgviewer->getOpenDirLevel();
    Config::view_feedpage   = imgviewer->getFeedPageMode();

    Config::pl_visible      = imgviewer->isPlaylistVisible();
}

