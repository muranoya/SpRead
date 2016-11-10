#ifndef MAGDIALOG_HPP
#define MAGDIALOG_HPP

#include <FL/Fl_Window.H>
#include <FL/Fl_Spinner.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include "Uncopyable.hpp"

class MagDialog : private Uncopyable
{
public:
    static bool getFactor(double org, double &rslt,
            const Fl_Window *parent = 0);

private:
    Fl_Window *w;
    Fl_Spinner *factor;
    Fl_Return_Button *btn_ok;
    Fl_Button *btn_cancel;
    double orgFactor;
    double rsltFactor;
    bool accepted;

    const int VPADDING;
    const int HPADDING;
    const int ITEM_HEIGHT;
    const int ITEM_PADDING;

    explicit MagDialog(int factor_org, const Fl_Window *parent = 0);
    ~MagDialog();

    static void pushedOkBtn(Fl_Widget *w, void *arg);
    static void pushedCancelBtn(Fl_Widget *w, void *arg);
};

#endif // MAGDIALOG_HPP
