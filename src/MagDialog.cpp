#include "MagDialog.hpp"

MagDialog::MagDialog()
    : w(nullptr)
    , factor(nullptr)
    , btn_ok(nullptr)
    , btn_cancel(nullptr)
    , accepted(false)
    , VPADDING(10)
    , HPADDING(10)
    , ITEM_HEIGHT(30)
    , ITEM_PADDING(7)
{
    w = new Fl_Window(150, 90, "Magnification");
    w->begin();
    {
        factor = new Fl_Spinner(
                w->w() - HPADDING - 60,
                VPADDING,
                60,
                ITEM_HEIGHT,
                "Factor:");
        factor->format("%0.2f");
        factor->step(0.05);
        factor->maximum(10);
        factor->minimum(0.01);

        btn_ok = new Fl_Button(
                w->w() - HPADDING - 60,
                factor->y() + factor->h() + VPADDING,
                60,
                ITEM_HEIGHT,
                "OK");
        btn_ok->when(FL_WHEN_RELEASE);
        btn_ok->callback(pushedOkBtn, this);
        btn_cancel = new Fl_Button(
                w->w() - HPADDING - 60 - HPADDING - 60,
                factor->y() + factor->h() + VPADDING,
                60,
                ITEM_HEIGHT,
                "Cancel");
        btn_cancel->when(FL_WHEN_RELEASE);
        btn_cancel->callback(pushedCancelBtn, this);
    }
    w->end();
}

MagDialog::~MagDialog()
{
    /*
    delete w;
    {
        delete factor;
        delete btn_ok;
        delete btn_cancel;
    }
    */
}

bool
MagDialog::getFactor(double ori, double &rslt)
{
    MagDialog dlg;
    dlg.originalFactor = ori;
    dlg.w->set_modal();
    dlg.w->show();
    while (dlg.w->shown()) Fl::wait();
    rslt = (dlg.accepted ? dlg.rsltFactor
                         : dlg.originalFactor);
    return dlg.accepted;
}

void
MagDialog::pushedOkBtn(Fl_Widget *w, void *arg)
{
    MagDialog *dlg = static_cast<MagDialog*>(arg);
    dlg->accepted = true;
    dlg->rsltFactor = dlg->factor->value();
    dlg->w->hide();
}

void
MagDialog::pushedCancelBtn(Fl_Widget *w, void *arg)
{
    MagDialog *dlg = static_cast<MagDialog*>(arg);
    dlg->accepted = false;
    dlg->w->hide();
}

