#include "App.hpp"
#include "Viewer.hpp"

using namespace std;

int App::mw_size_w;
int App::mw_size_h;
int App::mw_pos_x;
int App::mw_pos_y;

int       App::view_scalem;
double    App::view_scale;
int       App::view_ipix;
bool      App::view_spread;
bool      App::view_autospread;
bool      App::view_rbind;
int       App::view_openlevel;
const int App::view_openlevel_max = 99;
const int App::view_openlevel_min = 0;
int       App::view_feedpage;

bool      App::pl_visible;

const string App::SOFTWARE_ORG("muranoya.net");
const string App::SOFTWARE_NAME("SpRead");

void
App::SaveConfig()
{
    Fl_Preferences s(Fl_Preferences::USER,
            SOFTWARE_ORG.c_str(),
            SOFTWARE_NAME.c_str());

    Fl_Preferences mw(&s, "MainWindow");
    mw.set("size_w", mw_size_w);
    mw.set("size_h", mw_size_h);
    mw.set("pos_x",  mw_pos_x);
    mw.set("pos_y",  mw_pos_y);

    Fl_Preferences vw(&s, "Viewer");
    vw.set("scalem",     view_scalem);
    vw.set("scale",      view_scale);
    vw.set("ipix",       view_ipix);
    vw.set("spread",     view_spread);
    vw.set("autospread", view_autospread);
    vw.set("rbind",      view_rbind);
    vw.set("openlevel",  view_openlevel);
    vw.set("feedpage",   view_feedpage);

    Fl_Preferences pl(&s, "Playlist");
    pl.set("visible",  pl_visible);

    s.flush();
}

void
App::LoadConfig()
{
    Fl_Preferences s(Fl_Preferences::USER,
            SOFTWARE_ORG.c_str(),
            SOFTWARE_NAME.c_str());

    Fl_Preferences mw(&s, "MainWindow");
    mw.get("size_w", mw_size_w, 600);
    mw.get("size_h", mw_size_h, 400);
    mw.get("pos_x",  mw_pos_x,  100);
    mw.get("pos_y",  mw_pos_y,  100);

    Fl_Preferences vw(&s, "Viewer");
    vw.get("scalem",     view_scalem,     Viewer::FittingWindow);
    vw.get("scale",      view_scale,      1.0);
    vw.get("ipix",       view_ipix,       Viewer::Bilinear);
    g(vw,  "spread",     view_spread,     false);
    g(vw,  "autospread", view_autospread, false);
    g(vw,  "rbind",      view_rbind,      false);
    vw.get("openlevel",  view_openlevel,  99);
    checkRange(view_openlevel,
            view_openlevel_min,
            view_openlevel_max);
    vw.get("feedpage",   view_feedpage,   Viewer::MouseButton);

    Fl_Preferences pl(&s, "Playlist");
    g(pl,  "visible",  pl_visible,  true);
}

char
App::g(Fl_Preferences &s, const char *entry,
        bool &value, bool defaultValue)
{
    int x;
    char ret = s.get(entry, x, static_cast<int>(defaultValue));
    value = (x != 0);
    return ret;
}

void
App::checkRange(int &v, int min, int max)
{
    if (v < min) v = min;
    if (max < v) v = max;
}

