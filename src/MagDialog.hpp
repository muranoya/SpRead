#ifndef MAGDIALOG_HPP
#define MAGDIALOG_HPP

#include <FL/Fl_Window.H>
#include <FL/Fl_Spinner.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>

class MagDialog
{
public:
    static bool getFactor(double org, double &rslt);

private:
    Fl_Window *w;
    Fl_Spinner *factor;
    Fl_Return_Button *btn_ok;
    Fl_Button *btn_cancel;
    double originalFactor;
    double rsltFactor;
    bool accepted;

    const int VPADDING;
    const int HPADDING;
    const int ITEM_HEIGHT;
    const int ITEM_PADDING;

    explicit MagDialog();
    ~MagDialog();

    static void pushedOkBtn(Fl_Widget *w, void *arg);
    static void pushedCancelBtn(Fl_Widget *w, void *arg);
};

#endif // MAGDIALOG_HPP
