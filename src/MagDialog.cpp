#include "MagDialog.hpp"

MagDialog::MagDialog(int factor_org, const Fl_Window *parent)
    : w(nullptr)
    , factor(nullptr)
    , btn_ok(nullptr)
    , btn_cancel(nullptr)
    , orgFactor(factor_org)
    , rsltFactor(factor_org)
    , accepted(false)
    , VPADDING(10)
    , HPADDING(10)
    , ITEM_HEIGHT(30)
    , ITEM_PADDING(7)
{
    w = new Fl_Window(150, 90, "Magnification");
    w->set_modal();
    if (parent)
    {
        w->resize(parent->x()+parent->w()/2-w->w()/2,
                  parent->y()+parent->h()/2-w->h()/2,
                  w->w(), w->h());
    }
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

        btn_ok = new Fl_Return_Button(
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
    delete w;
}

bool
MagDialog::getFactor(double org, double &rslt, const Fl_Window *parent)
{
    MagDialog dlg(org, parent);
    dlg.w->show();
    while (dlg.w->shown()) Fl::wait();
    rslt = (dlg.accepted ? dlg.rsltFactor
                         : dlg.orgFactor);
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

