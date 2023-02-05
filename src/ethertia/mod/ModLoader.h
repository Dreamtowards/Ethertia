//
// Created by Dreamtowards on 2023/1/31.
//

#ifndef ETHERTIA_MODLOADER_H
#define ETHERTIA_MODLOADER_H

#ifdef __APPLE__
#include <dlfcn.h>
#elif __WIN32__
#include <windows.h>
#endif

#include <ethertia/mod/Mod.h>
#include <ethertia/util/Log.h>

#include <nlohmann/json.hpp>

class ModLoader
{
public:
    using InitFunc = void(*)();

    static void loadMod(const std::string& modpath)
    {
        using nlohmann::json;

        Mod::Manifest manifest;
        json data = json::parse(Loader::loadFile(modpath + "/mod.json").new_string());

        manifest.name = data["name"];
        manifest.id = data["id"];
        manifest.version = data["version"];
        manifest.website = data["website"];
    }

    static bool loadProgram(const char* file)
    {

#ifdef __APPLE__

        void* handle = dlopen(file, RTLD_LAZY);
        if (!handle) {
            Log::warn("Unable to load mod program ({}).", file);
            return false;
        }

        InitFunc func = (InitFunc)dlsym(handle, "init");

        if (!func) {
            Log::warn("Unable to load init func. ({})", file);
            return false;
        }

        func();

        dlclose(handle);

        return true;

#elif __WIN32__

        // https://stackoverflow.com/questions/72249298/dynamic-load-a-class-from-a-dll-in-windows

//        void* handle = LoadLibrary(file.c_str());
//        if (!handle) {
//            // error
//        }
//
//        InitFunc f = GetProcAddress((HINSTANCE)handle, "init");
//
//         FreeLibrary((HMODULE)handle);
#endif

    }

};

#endif //ETHERTIA_MODLOADER_H
