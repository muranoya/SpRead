#include <FL/Fl.H>
#include "Config.hpp"
#include "MainWindow.hpp"

int
main(int argc, char *argv[])
{
    Config::LoadConfig();
    MainWindow *mw = new MainWindow(argc, argv,
            Config::mw_pos_x,  Config::mw_pos_y,
            Config::mw_size_w, Config::mw_size_h);
    Fl::visual(FL_RGB);
    mw->show();
    Fl::lock();
    return Fl::run();
}

