//
// Created by Dreamtowards on 2023/3/8.
//

#include "Settings.h"

#include <nlohmann/json.hpp>
#include <stdx/collection.h>

#include <fstream>
#include <format>
#include <map>
#include <filesystem>

#include <ethertia/util/Loader.h>
#include <ethertia/util/BenchmarkTimer.h>
#include <ethertia/Ethertia.h>
#include <ethertia/init/MaterialTextures.h>
#include <ethertia/render/Window.h>
#include <ethertia/imgui/Imgui.h>
#include <ethertia/imgui/Imw.h>
#include <ethertia/imgui/ImwInspector.h>
//#include <ethertia/world/gen/NoiseGen.h>


static const char* SETTINGS_FILE = "./settings.json";






void Settings::LoadSettings()
{
    // Hack: attempts modify Working Directory. from e.g. .../bin/windows-x64/ to .../
    if (std::filesystem::current_path().parent_path().filename() == "bin")
    {
        std::filesystem::current_path(std::filesystem::current_path().parent_path().parent_path());
    }

    // Check working directory
    if (!Loader::FileExists("./assets"))
        throw std::runtime_error(std::format("default assets directory not found. make sure you are in valid working directory. (current dir: {})", std::filesystem::current_path().string()));

    auto& DrawFuncIds = Imgui::DrawFuncIds;

    DrawFuncIds[Imw::Settings::ShowSettings] = "settings";
    DrawFuncIds[ImwInspector::ShowInspector] = "inspector";
    DrawFuncIds[ImwInspector::ShowHierarchy] = "hierarchy";
    DrawFuncIds[ImwGame::ShowWorldSettings] = "worldsettings";
    DrawFuncIds[Imw::Editor::ShowProfiler] = "profiler";
    DrawFuncIds[Imw::Editor::ShowToolbar] = "toolbar";
    DrawFuncIds[Imw::Editor::ShowConsole] = "console";
    DrawFuncIds[Imw::Editor::ShowExplorer] = "explorer";
    DrawFuncIds[Imw::Editor::ShowWorldGen] = "worldgen";
    DrawFuncIds[ImwGame::ShowGame] = "game";
    DrawFuncIds[ImwGame::ShowTitleScreen] = "game-titlescreen";
    DrawFuncIds[ImwGame::ShowWorldList] = "game-worldlist";
    DrawFuncIds[ImwGame::ShowWorldNew] = "game-worldnew";
    DrawFuncIds[ImwGame::ShowTitleScreen] = "game-titlescreen";
    DrawFuncIds[ImGui::ShowDemoWindow] = "dbg-imgui-demowindow";


    BENCHMARK_TIMER;
    Log::info("Load Settings.\1");
    if (!Loader::FileExists(SETTINGS_FILE)) {
        Log::info("settings file {} not found. skip.", SETTINGS_FILE);
        return;
    }


    using namespace nlohmann;
    json conf = json::parse((char*)Loader::LoadFile(SETTINGS_FILE).data());


    conf.get("view_distance", s_ViewDistance);
    conf.get("fov", Ethertia::GetCamera().fov);
    conf.get("display_width", DisplayWidth);
    conf.get("display_height", DisplayHeight);
    conf.get("vsync", s_Vsync);

    conf.get("assets", Settings::Assets);
    conf.get("mods", Settings::Mods);

    conf.get("mtl_resolution", MaterialTextures::TEX_RESOLUTION);

    conf.get("graphics.ssao", g_SSAO);
    conf.get("graphics.shadow", g_ShadowMapping);

    //std::string simdLevel;
    //if (conf.get("simd_level", simdLevel))
    //{
    //    NoiseGen::g_SIMDLevel = NoiseGen::FastSIMD_ofLevelName(simdLevel);
    //}


    std::vector<std::string> openedwindows;
    if (conf.get("windows", openedwindows))
    {
        for (const std::string& k : openedwindows)
        {
            //auto it = DrawFuncIds.find(k);
            //if (it == DrawFuncIds.end()) {
            //    Log::warn("failed to load unknown window {}", k);
            //} else {
                Imgui::Show(stdx::find_key(DrawFuncIds, k));
            //}
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
    conf["fov"] =           Ethertia::GetCamera().fov;
    conf["assets"] =        Settings::Assets;
    conf["display_width"]  =_WindowSize.x;
    conf["display_height"] =_WindowSize.y;
    conf["vsync"] =         s_Vsync;
    conf["mtl_resolution"] =MaterialTextures::TEX_RESOLUTION;
    //conf["simd_level"] =    NoiseGen::FastSIMD_LevelName(NoiseGen::g_SIMDLevel);
    conf["mods"] =          Settings::Mods;
    conf["graphics.ssao"] = Settings::g_SSAO;
    conf["graphics.shadow"]=Settings::g_ShadowMapping;


    std::vector<std::string> openedwindows;
    for (auto& ptr : Imgui::DrawFuncList) {
        openedwindows.push_back(Imgui::DrawFuncIds[ptr]);
    }
    conf["windows"] =       openedwindows;


    std::ofstream f(SETTINGS_FILE);
    f << conf.dump(2);
    f.close();
}