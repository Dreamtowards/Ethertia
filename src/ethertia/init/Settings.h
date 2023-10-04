//
// Created by Dreamtowards on 2022/12/17.
//

#pragma once

#include <string>
#include <vector>


class Settings
{
public:
    inline static int DisplayWidth  = 1280,
                      DisplayHeight = 720;

    inline static std::vector<std::string> Mods;

    // Resource Packs. locate assets file by Loader::fileAssets() ordered.   // fixme: might conflict with mods?
    inline static std::vector<std::string> Assets = {"assets/"};  // default value, would be replace by settings.json::assets


    static void LoadSettings();

    static void SaveSettings();



    inline static bool g_SSAO = true,
                       g_ShadowMapping = true;


    inline static bool w_Console_FocusInput = false;  // true: Force bring Console Window to front and Focus on the InputBox while Console Window was showing. used when press '/' key.


    inline static float s_ViewDistance = 4;
    inline static int   s_FpsCap = 60;
    inline static bool  s_Vsync = true;

    // Interval(sec) of Breaking Terrain in Creative Mode.
    //inline static float gInterval_BreakingTerrain_CreativeMode = 0.2;


    // Interval(sec) of Re-Render ShadowMap's DepthMap. (which kanda expensive.)
    inline static float gInterval_ShadowDepthMap = 0.4f;


};

