#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <FL/Fl_Preferences.H>
#include <string>

class Config
{
public:
    // Group - MainWindow
    static int mw_size_w;
    static int mw_size_h;
    static int mw_pos_x;
    static int mw_pos_y;

    // Group - Viewer
    static int    view_scalem;
    static double view_scale;
    static int    view_ipix;
    static bool   view_spread;
    static bool   view_autospread;
    static bool   view_rbind;
    static int    view_openlevel;
    static const int view_openlevel_max;
    static const int view_openlevel_min;
    static int    view_feedpage;
    static bool   view_monomode;

    // Group - Playlist
    static bool pl_visible;

    static const std::string SOFTWARE_ORG;
    static const std::string SOFTWARE_NAME;

    static void SaveConfig();
    static void LoadConfig();

private:
    Config() = delete;

    static char g(Fl_Preferences &s,
            const char *entry, bool &value,
            bool defaultValue);
    static void checkRange(int &v, int min, int max);
};

#endif // CONFIG_HPP
