//
// Created by Dreamtowards on 2023/3/8.
//

#include "Settings.h"

#include <nlohmann/json.hpp>
#include <stdx/stdx.h>

#include <fstream>
#include <format>
#include <map>
#include <filesystem>

#include <ethertia/util/Loader.h>
#include <ethertia/world/gen/NoiseGen.h>
#include <ethertia/init/MaterialTextures.h>
#include <ethertia/render/Window.h>
#include <ethertia/imgui/Imgui.h>
#include <ethertia/imgui/Imw.h>


static const char* SETTINGS_FILE = "./settings.json";


// for Store & Restore opened windows.
static std::map<std::string, Imgui::DrawFuncPtr> DrawFuncIds;




void Settings::LoadSettings()
{
    if (!Loader::FileExists("./assets"))
        throw std::runtime_error(std::format("default assets directory not found. make sure you are in valid working directory. (current dir: {})", std::filesystem::current_path().string()));

    DrawFuncIds["settings"] = Imw::Settings::ShowSettings;
    DrawFuncIds["inspector"] = Imw::Editor::ShowInspector;
    DrawFuncIds["hierarchy"] = Imw::Editor::ShowHierarchy;
    DrawFuncIds["profiler"] = Imw::Editor::ShowProfiler;
    DrawFuncIds["toolbar"] = Imw::Editor::ShowToolbar;
    DrawFuncIds["console"] = Imw::Editor::ShowConsole;
    DrawFuncIds["explorer"] = Imw::Editor::ShowExplorer;
    DrawFuncIds["worldgen"] = Imw::Editor::ShowWorldGen;
    DrawFuncIds["game"] = Imw::Gameplay::ShowGame;
    DrawFuncIds["game-titlescreen"] = Imw::Gameplay::ShowTitleScreen;
    DrawFuncIds["game-worldlist"] = Imw::Gameplay::ShowWorldList;
    DrawFuncIds["game-worldnew"] = Imw::Gameplay::ShowWorldNew;
    DrawFuncIds["game-titlescreen"] = Imw::Gameplay::ShowTitleScreen;
    DrawFuncIds["dbg-imgui-demowindow"] = ImGui::ShowDemoWindow;


    BENCHMARK_TIMER;
    Log::info("Load Settings.\1");
    if (!Loader::FileExists(SETTINGS_FILE)) {
        Log::info("settings file {} not found. skip.", SETTINGS_FILE);
        return;
    }


    using namespace nlohmann;
    json conf = json::parse((char*)Loader::LoadFile(SETTINGS_FILE).data());


    conf.get("view_distance", s_ViewDistance);
    conf.get("fov", Ethertia::getCamera().fov);
    conf.get("display_width", DisplayWidth);
    conf.get("display_height", DisplayHeight);
    conf.get("vsync", s_Vsync);

    conf.get("assets", Settings::Assets);
    conf.get("mods", Settings::Mods);

    conf.get("mtl_resolution", MaterialTextures::TEX_RESOLUTION);

    conf.get("graphics.ssao", g_SSAO);
    conf.get("graphics.shadow", g_ShadowMapping);

    std::string simdLevel;
    if (conf.get("simd_level", simdLevel))
    {
        NoiseGen::g_SIMDLevel = NoiseGen::FastSIMD_ofLevelName(simdLevel);
    }


    std::vector<std::string> openedwindows;
    if (conf.get("windows", openedwindows))
    {
        for (const std::string& k : openedwindows)
        {
            auto it = DrawFuncIds.find(k);
            if (it == DrawFuncIds.end()) {
                Log::warn("failed to load unknown window {}", k);
            } else {
                Imgui::Show(it->second);
            }
        }
    }


}





void Settings::SaveSettings()
{
    BENCHMARK_TIMER;
    Log::info("Save Settings.\1");

    using namespace nlohmann;
    json conf = json{};

    glm::vec2 _WindowSize = Window::Size();

    conf["view_distance"] = s_ViewDistance;
    conf["fov"] =           Ethertia::getCamera().fov;
    conf["assets"] =        Settings::Assets;
    conf["display_width"]  =_WindowSize.x;
    conf["display_height"] =_WindowSize.y;
    conf["vsync"] =         s_Vsync;
    conf["mtl_resolution"] =MaterialTextures::TEX_RESOLUTION;
    conf["simd_level"] =    NoiseGen::FastSIMD_LevelName(NoiseGen::g_SIMDLevel);
    conf["mods"] =          Settings::Mods;
    conf["graphics.ssao"] = Settings::g_SSAO;
    conf["graphics.shadow"]=Settings::g_ShadowMapping;


    std::vector<std::string> openedwindows;
    for (auto& ptr : Imgui::DrawFuncList) {
        openedwindows.push_back(stdx::find_key(DrawFuncIds, ptr));
    }
    conf["windows"] =       openedwindows;


    std::ofstream f(SETTINGS_FILE);
    f << conf.dump(2);
    f.close();
}