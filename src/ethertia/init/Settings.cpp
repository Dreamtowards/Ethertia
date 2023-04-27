//
// Created by Dreamtowards on 2023/3/8.
//



#include <ethertia/init/Settings.h>

#include <ethertia/util/Loader.h>
#include <ethertia/world/gen/NoiseGen.h>
#include <ethertia/render/RenderEngine.h>
#include <ethertia/init/MaterialTextures.h>
#include <ethertia/Ethertia.h>
#include <ethertia/render/Window.h>

#include <nlohmann/json.hpp>


template<typename T>
static void set_ifexists(nlohmann::json& j, const std::string& name, T* p) {
    if (j.contains(name)) {
        *p = j[name];
    }
}

void Settings::loadSettings() {  using nlohmann::json;
    BENCHMARK_TIMER;
    Log::info("Load Settings.\1");
    if (!Loader::fileExists(SETTINGS_FILE))
        return;

    json settings = json::parse((char*)Loader::loadFile(SETTINGS_FILE).data());

    set_ifexists(settings, "view_distance",  &s_ViewDistance);
    set_ifexists(settings, "fov",            &Ethertia::getCamera().fov);
    set_ifexists(settings, "display_width",  &displayWidth);
    set_ifexists(settings, "display_height", &displayHeight);
    set_ifexists(settings, "vsync",          &s_Vsync);

    set_ifexists(settings, "assets",         &Settings::ASSETS);
    set_ifexists(settings, "mods",           &MODS);


    std::string simdLevel;
    set_ifexists(settings, "simd_level", &simdLevel);
    NoiseGen::g_SIMDLevel = NoiseGen::FastSIMD_ofLevelName(simdLevel);

    set_ifexists(settings, "mtl_resolution", &MaterialTextures::TEX_RESOLUTION);

    set_ifexists(settings, "graphics.ssao", &g_SSAO);
    set_ifexists(settings, "graphics.shadow", &g_ShadowMapping);

    set_ifexists(settings, "window.viewport", &w_Viewport);
    set_ifexists(settings, "window.settings", &w_Settings);
    set_ifexists(settings, "window.toolbar", &w_Toolbar);
    set_ifexists(settings, "window.console", &w_Console);

    set_ifexists(settings, "window.entity_list", &w_EntityList);
    set_ifexists(settings, "window.entity_insp", &w_EntityInsp);
    set_ifexists(settings, "window.shader_insp", &w_ShaderInsp);


}


void Settings::saveSettings()  {  using nlohmann::json;
    BENCHMARK_TIMER;
    Log::info("Save Settings.\1");

    json settings = json::object({
        {"view_distance",  s_ViewDistance},
        {"fov",            Ethertia::getCamera().fov},
        {"assets",         Settings::ASSETS},
        {"display_width",  Ethertia::getWindow().getWidth()},
        {"display_height", Ethertia::getWindow().getHeight()},
        {"vsync",          s_Vsync},
        {"fullscreen",     false},
        {"mtl_resolution", MaterialTextures::TEX_RESOLUTION},
        {"simd_level",     NoiseGen::FastSIMD_LevelName(NoiseGen::g_SIMDLevel)},
        {"mods",           MODS},
        {"graphics.ssao",  Settings::g_SSAO},
        {"graphics.shadow",Settings::g_ShadowMapping},
        {"window.viewport",w_Viewport},
        {"window.settings",w_Settings},
        {"window.toolbar", w_Toolbar},
        {"window.console", w_Console},
        {"window.entity_list", w_EntityList},
        {"window.entity_insp", w_EntityInsp},
        {"window.shader_insp", w_ShaderInsp}
    });

    std::ofstream f(SETTINGS_FILE);
    f << settings.dump(2);
}