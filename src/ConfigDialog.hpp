#ifndef CONFIGDIALOG_HPP
#define CONFIGDIALOG_HPP

#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Spinner.H>
#include <FL/Fl_Radio_Round_Button.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include "Uncopyable.hpp"

class ConfigDialog : private Uncopyable
{
public:
    static bool openConfigDialog(const Fl_Window *parent = 0);

private:
    Fl_Window *w;
    Fl_Group *gOpenDir;
    Fl_Spinner *opendirSpinner;
    Fl_Group *gPlaylist;
    Fl_Radio_Round_Button *plRBtn_ClickBtn;
    Fl_Radio_Round_Button *plRBtn_ClickPos;
    Fl_Return_Button *btn_ok;
    Fl_Button *btn_cancel;
    bool accepted;

    const int VPADDING;
    const int HPADDING;
    const int ITEM_HEIGHT;
    const int ITEM_PADDING;

    explicit ConfigDialog(const Fl_Window *parent = 0);
    ~ConfigDialog();

    static void pushedOkBtn(Fl_Widget *w, void *arg);
    static void pushedCancelBtn(Fl_Widget *w, void *arg);

    void loadSettings();
    void saveSettings();
};

#endif // CONFIGDIALOG_HPP
