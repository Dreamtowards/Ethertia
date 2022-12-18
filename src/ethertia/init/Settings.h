//
// Created by Dreamtowards on 2022/12/17.
//

#ifndef ETHERTIA_SETTINGS_H
#define ETHERTIA_SETTINGS_H

#include <ethertia/util/Loader.h>

#include <nlohmann/json.hpp>

class Settings
{
    using json = nlohmann::json;
public:
    inline static const char* SETTINGS_FILE = "./settings.json";

//    // init properties.
//
//    inline static int viewDistance = 3;
//    inline static float fov = 90;
//
//    inline static std::vector<std::string> assets = {"./assets"};
//
    inline static int displayWidth  = 1280,
                      displayHeight = 720;

    template<typename T>
    static void setProperty(json& j, const std::string& name, T* p) {
        if (j.contains(name)) {
            *p = j[name];
        }
    }

    static void loadSettings() { BenchmarkTimer _tm;
        Log::info("Load Settings.\1");
        if (!Loader::fileExists(SETTINGS_FILE))
            return;

        json settings = json::parse(Loader::loadFileStr(SETTINGS_FILE));

        setProperty(settings, "view_distance", &RenderEngine::viewDistance);
        setProperty(settings, "fov", &RenderEngine::fov);

        setProperty(settings, "assets", &Loader::ASSETS);

        setProperty(settings, "display_width",  &displayWidth);
        setProperty(settings, "display_height", &displayHeight);

    }

    static void loadProgramArguments(int argc, const char* argv[]) {

    }

    static void saveSettings() { BenchmarkTimer _tm;
        Log::info("Save Settings.\1");

        json settings = json::object({
             {"view_distance",  RenderEngine::viewDistance},
             {"fov",            RenderEngine::fov},
             {"assets",         Loader::ASSETS},
             {"display_width",  Ethertia::getWindow()->getWidth()},
             {"display_height", Ethertia::getWindow()->getHeight()},
             {"vsync",          false},
             {"fullscreen",     false},
        });

        std::ofstream f(SETTINGS_FILE);
        f << settings.dump(4);
    }


};

#endif //ETHERTIA_SETTINGS_H
