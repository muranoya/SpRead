#ifndef OPENDIALOG_HPP
#define OPENDIALOG_HPP

#include <FL/Fl_Window.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Button.H>
#include "Uncopyable.hpp"
#include "SpRead.hpp"

class OpenDialog : private Fl_Window
{
public:
    explicit OpenDialog(const Fl_Window *parent = 0);
    ~OpenDialog();

    void showDialog();
    THREAD_SAFE_FUNC void asyncDone();

private:
    bool done;

    void hide();
};

#endif // OPENDIALOG_HPP
