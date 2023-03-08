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

    json settings = json::parse(Loader::loadFile(SETTINGS_FILE).new_string());

    set_ifexists(settings, "view_distance",  &RenderEngine::viewDistance);
    set_ifexists(settings, "fov",            &RenderEngine::fov);

    set_ifexists(settings, "assets",         &Loader::ASSETS);

    set_ifexists(settings, "display_width",  &displayWidth);
    set_ifexists(settings, "display_height", &displayHeight);

    set_ifexists(settings, "mtl_resolution", &MaterialTextures::TEX_RESOLUTION);

    std::string simdLevel;
    set_ifexists(settings, "simd_level", &simdLevel);
    NoiseGen::g_SIMDLevel = NoiseGen::FastSIMD_ofLevelName(simdLevel);

    set_ifexists(settings, "mods", &MODS);

}


void Settings::saveSettings()  {  using nlohmann::json;
    BENCHMARK_TIMER;
    Log::info("Save Settings.\1");

    json settings = json::object({
        {"view_distance",  RenderEngine::viewDistance},
        {"fov",            RenderEngine::fov},
        {"assets",         Loader::ASSETS},
        {"display_width",  Ethertia::getWindow()->getWidth()},
        {"display_height", Ethertia::getWindow()->getHeight()},
        {"vsync",          false},
        {"fullscreen",     false},
        {"mtl_resolution", MaterialTextures::TEX_RESOLUTION},
        {"simd_level",     NoiseGen::FastSIMD_LevelName(NoiseGen::g_SIMDLevel)},
        {"mods",           MODS}
    });

    std::ofstream f(SETTINGS_FILE);
    f << settings.dump(2);
}