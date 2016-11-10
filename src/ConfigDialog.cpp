#include <FL/Fl.H>
#include "ConfigDialog.hpp"
#include "Config.hpp"
#include "Viewer.hpp"

ConfigDialog::ConfigDialog(const Fl_Window *parent)
    : w(nullptr)
    , gOpenDir(nullptr)
    , opendirSpinner(nullptr)
    , gPlaylist(nullptr)
    , plRBtn_ClickBtn(nullptr)
    , plRBtn_ClickPos(nullptr)
    , btn_ok(nullptr)
    , btn_cancel(nullptr)
    , accepted(false)
    , VPADDING(20)
    , HPADDING(10)
    , ITEM_HEIGHT(30)
    , ITEM_PADDING(7)
{
    w = new Fl_Window(290, 205, "Config");
    w->set_modal();
    if (parent)
    {
        w->resize(parent->x()+parent->w()/2-w->w()/2,
                  parent->y()+parent->h()/2-w->h()/2,
                  w->w(), w->h());
    }
    w->begin();
    {
        const int GROUP_W = w->w() - HPADDING*2;
#define GROUP_H(N) (ITEM_HEIGHT*(N)+ITEM_PADDING*2)
        const int GROUP_X = HPADDING;
        int GROUP_Y = VPADDING;

#define ITEM_X(GBOX, W)   ((GBOX)->x()+(GBOX)->w()-(W)-ITEM_PADDING)
#define ITEM_Y(GBOX, IDX) ((GBOX)->y()+(IDX)*ITEM_HEIGHT+ITEM_PADDING)
#define ITEM_W(GBOX)      ((GBOX)->w()-ITEM_PADDING*2)


        gOpenDir = new Fl_Group(
                GROUP_X, GROUP_Y,
                GROUP_W, GROUP_H(1),
                "Open");
        gOpenDir->box(FL_EMBOSSED_BOX);
        gOpenDir->begin();
        {
            opendirSpinner = new Fl_Spinner(
                    ITEM_X(gOpenDir, 60),
                    ITEM_Y(gOpenDir, 0),
                    60,
                    ITEM_HEIGHT,
                    "Directory Depth:");
            opendirSpinner->maximum(
                    static_cast<double>(Config::view_openlevel_max));
            opendirSpinner->minimum(
                    static_cast<double>(Config::view_openlevel_min));
        }
        gOpenDir->end();
        GROUP_Y += gOpenDir->h() + VPADDING;


        gPlaylist = new Fl_Group(
                GROUP_X, GROUP_Y,
                GROUP_W, GROUP_H(2),
                "Navigation");
        gPlaylist->box(FL_EMBOSSED_BOX);
        gPlaylist->begin();
        {
            plRBtn_ClickBtn = new Fl_Radio_Round_Button(
                    ITEM_X(gPlaylist, ITEM_W(gPlaylist)),
                    ITEM_Y(gPlaylist, 0),
                    ITEM_W(gPlaylist), ITEM_HEIGHT,
                    "Next/Prev Images by Left/Right Click");
            plRBtn_ClickPos = new Fl_Radio_Round_Button(
                    ITEM_X(gPlaylist, ITEM_W(gPlaylist)),
                    ITEM_Y(gPlaylist, 1),
                    ITEM_W(gPlaylist), ITEM_HEIGHT,
                    "Click in the Left/Right Half");
        }
        gPlaylist->end();
        GROUP_Y += gPlaylist->h() + VPADDING;


        btn_ok = new Fl_Return_Button(
                w->w() - HPADDING - 60 - HPADDING,
                GROUP_Y-10,
                60, ITEM_HEIGHT,
                "&OK");
        btn_ok->when(FL_WHEN_RELEASE);
        btn_ok->callback(pushedOkBtn, this);
        btn_cancel = new Fl_Button(
                w->w() - HPADDING - 60 - HPADDING - 60 - HPADDING,
                GROUP_Y-10,
                60, ITEM_HEIGHT,
                "&Cancel");
        btn_cancel->when(FL_WHEN_RELEASE);
        btn_cancel->callback(pushedCancelBtn, this);
#undef GROUP_H
#undef ITEM_Y
#undef ITEM_X
#undef ITEM_W
    }
    w->end();

    loadSettings();
}

ConfigDialog::~ConfigDialog()
{
    delete w;
}

bool
ConfigDialog::openConfigDialog(const Fl_Window *parent)
{
    ConfigDialog dlg(parent);
    dlg.w->show();
    while (dlg.w->shown()) Fl::wait();
    return dlg.accepted;
}

void
ConfigDialog::pushedOkBtn(Fl_Widget *w, void *arg)
{
    ConfigDialog *dlg = static_cast<ConfigDialog*>(arg);
    dlg->saveSettings();
    dlg->accepted = true;
    dlg->w->hide();
}

void
ConfigDialog::pushedCancelBtn(Fl_Widget *w, void *arg)
{
    ConfigDialog *dlg = static_cast<ConfigDialog*>(arg);
    dlg->accepted = false;
    dlg->w->hide();
}

void
ConfigDialog::loadSettings()
{
    opendirSpinner->value(Config::view_openlevel);
    plRBtn_ClickBtn->value(
            Config::view_feedpage == Viewer::MouseButton);
    plRBtn_ClickPos->value(
            Config::view_feedpage == Viewer::MouseClickPosition);
}

void
ConfigDialog::saveSettings()
{
    Config::view_openlevel = static_cast<int>(opendirSpinner->value());
    if (plRBtn_ClickBtn->value())
    {
        Config::view_feedpage = Viewer::MouseButton;
    }
    else
    {
        Config::view_feedpage = Viewer::MouseClickPosition;
    }
}

