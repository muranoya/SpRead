#include <FL/Fl.H>
#include "App.hpp"
#include "MainWindow.hpp"

int
main(int argc, char *argv[])
{
    App::LoadConfig();
    MainWindow *mw = new MainWindow(argc, argv,
            App::mw_pos_x, App::mw_pos_y,
            App::mw_size_w, App::mw_size_h);
    Fl::visual(FL_RGB);
    mw->show();
    return Fl::run();
}

