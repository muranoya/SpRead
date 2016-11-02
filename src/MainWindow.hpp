#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Menu_Item.H>
#include "ImageViewer.hpp"

class MainWindow : public Fl_Double_Window
{
public:
    explicit MainWindow(int argc, char *argv[],
            int x, int y, int w, int h);
    ~MainWindow();

    void hide();

private:
    Fl_Menu_Bar *menubar;
    ImageViewer *imgviewer;

    static void file_open        (Fl_Widget *w, void *arg);
    static void file_opendir     (Fl_Widget *w, void *arg);
    static void file_config      (Fl_Widget *w, void *arg);
    static void file_exit        (Fl_Widget *w, void *arg);

    static void view_actualsize  (Fl_Widget *w, void *arg);
    static void view_fitwindow   (Fl_Widget *w, void *arg);
    static void view_fitwidth    (Fl_Widget *w, void *arg);
    static void view_specmag     (Fl_Widget *w, void *arg);
    static void view_spread      (Fl_Widget *w, void *arg);
    static void view_autospread  (Fl_Widget *w, void *arg);
    static void view_rightbinding(Fl_Widget *w, void *arg);
    static void view_nn          (Fl_Widget *w, void *arg);
    static void view_bl          (Fl_Widget *w, void *arg);
    static void view_bc          (Fl_Widget *w, void *arg);

    static void window_playlist(Fl_Widget *w, void *arg);

    void updateWindowTitle();

    Fl_Menu_Item *findMenuItem(Fl_Callback *callback);
    void createMenus();
    void applyConfig();
    void storeConfig();
};

#endif // MAINWINDOW_HPP
