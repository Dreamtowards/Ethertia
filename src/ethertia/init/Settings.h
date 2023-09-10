//
// Created by Dreamtowards on 2022/12/17.
//

#ifndef ETHERTIA_SETTINGS_H
#define ETHERTIA_SETTINGS_H

#include <string>
#include <vector>

class Settings
{
public:
    inline static const char* SETTINGS_FILE = "./settings.json";


    inline static int displayWidth  = 1280,
                      displayHeight = 720;

    inline static std::vector<std::string> MODS;

    // Resource Packs. locate assets file by Loader::fileAssets() ordered.   // fixme: might conflict with mods?
    inline static std::vector<std::string> ASSETS = {"assets/"};  // default value, would be replace by settings.json::assets


    static void loadSettings();

    static void saveSettings();

    //static void loadProgramArguments(int argc, const char* argv[]) {}


    inline static bool g_SSAO = true,
                       g_ShadowMapping = true;


    inline static bool w_Viewport_Full = false;
    inline static bool w_Console_FocusInput = false;  // true: Force bring Console Window to front and Focus on the InputBox while Console Window was showing. used when press '/' key.


    inline static float s_ViewDistance = 4;
    inline static int   s_FpsCap = 60;
    inline static bool  s_Vsync = true;

    // Interval(sec) of Breaking Terrain in Creative Mode.
    inline static float gInterval_BreakingTerrain_CreativeMode = 0.2;


    // Interval(sec) of Re-Render ShadowMap's DepthMap. (which kanda expensive.)
    inline static float gInterval_ShadowDepthMap = 0.4f;


};

#endif //ETHERTIA_SETTINGS_H
