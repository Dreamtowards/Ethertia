//
// Created by Dreamtowards on 2023/1/31.
//
// dll load for Windows created by Kmroiosn on 2023/02/09. (D)
//

#ifndef ETHERTIA_MODLOADER_H
#define ETHERTIA_MODLOADER_H

#if defined(__APPLE__)|| defined(__linux)
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
        BENCHMARK_TIMER;
        using nlohmann::json;

        Mod::Manifest manifest;
        json data = json::parse((char*)Loader::loadFile(modpath + "/mod.json").data());

        manifest.name = data["name"];
        manifest.id = data["id"];

        if (data.contains("version")) manifest.version = data["version"];
        if (data.contains("website")) manifest.version = data["website"];

        std::string nameCompact = manifest.name; Strings::erase(nameCompact, ' ');
        auto conf_bin = data["bin"];
        std::string binPath = conf_bin[Loader::sys_target()];
        {
            bool succ = loadModProgram(binPath.c_str());

            assert(succ && "Failed to load mod program.");
        }

        Log::info("Mod {} ({}, {}) loaded in\1", manifest.name, manifest.id, binPath);
    }






    // <ModDir>/bin/<windows|darwin>-<x64|arm64>
//    static std::string _ModProgramPath(const std::string& modpath, const std::string& name) {
//        return modpath + "/bin/" + Loader::sys_target_name() + "/" + Loader::sys_lib_name(name);
//    }

    static bool loadModProgram(const char* file)
    {
#if defined(__APPLE__)|| defined(__linux)

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

#elif _WIN32
        // https://stackoverflow.com/questions/72249298/dynamic-load-a-class-from-a-dll-in-windows

        // https://learn.microsoft.com/zh-cn/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibrarya
        // DLL path should use '\' instead of '/', but still works fine
        /* Replace every '/' to '\'
        std::string path = file;
        for (size_t pos = 0; pos != std::string::npos; pos++) {
            if ((pos = path.find('/', pos)) != std::string::npos)
                path.replace(pos, 1, "\\");
            else break;
        }
        */

        HINSTANCE handle = LoadLibrary(file);

        if (handle == nullptr) {
            // Retrieve the system error message for the last-error code
            LPSTR* lpMsgBuf;
            DWORD errCode = GetLastError();

            FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr,
                errCode,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR) &lpMsgBuf,
                0, nullptr
                );

            Log::warn("Error occurred while loading mod from {}, {}", file, (const char*)lpMsgBuf);
            LocalFree(lpMsgBuf);
            return false;
        }

        InitFunc func = (InitFunc)GetProcAddress(handle, "init");

        if (!func) {
            Log::warn("Unable to load init func. ({})", file);
            return false;
        }

        func();

        FreeLibrary(handle);
        return true;
#else
        static_assert(false);
#endif

    }

};

#endif //ETHERTIA_MODLOADER_H
