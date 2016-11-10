#include <FL/Fl.H>
#include "OpenDialog.hpp"

using namespace std;

OpenDialog::OpenDialog(const Fl_Window *parent)
    : Fl_Window(150, 70, "Loading...")
    , done(false)
{
    end();
    set_modal();
    if (parent)
    {
        resize(parent->x()+parent->w()/2-w()/2,
               parent->y()+parent->h()/2-h()/2,
               w(), h());
    }
}

OpenDialog::~OpenDialog()
{
}

void
OpenDialog::showDialog()
{
    show();
    while (!done) Fl::wait(0.03);
}

THREAD_SAFE_FUNC void
OpenDialog::asyncDone()
{
    done = true;
}

void
OpenDialog::hide()
{
}

