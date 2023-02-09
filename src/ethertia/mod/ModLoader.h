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

    static std::string mapLibraryFile(const std::string& name) {
#if __APPLE__
        return "lib" + name + ".dylib";
#elif __WIN32__
        return "lib" + name + ".dll";
#endif
    }
    static std::string sysTargetName() {
#if defined(__APPLE__) && defined(__x86_64__)
        return "darwin-x64";
#elif defined(__APPLE__) && defined(__aarch64__)
        return "darwin-arm64";
#elif defined(__WIN32__) && defined(__x86_64__)
        return "windows-x64";
#endif
    }

    static std::string getModProgramPath(const std::string& name) {
        return "bin/" + sysTargetName() + "/" + mapLibraryFile(name);
    }

    static void loadMod(const std::string& modpath)
    {
        using nlohmann::json;

        Mod::Manifest manifest;
        json data = json::parse(Loader::loadFile(modpath + "/mod.json").new_string());

        manifest.name = data["name"];
        manifest.id = data["id"];
        manifest.version = data["version"];
        // manifest.website = data["website"];

        // bin/darwin-<x64|arm64>
        // bin/windows-<x64>

//        loadProgram(getModProgramPath(manifest.name).c_str());
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

        InitFunc f = (InitFunc)GetProcAddress(handle, "init");

        if (!f) {
            Log::warn("Unable to load init func. ({})", file);
            return false;
        }

        f();

        FreeLibrary(handle);
        return true;
#endif

    }

};

#endif //ETHERTIA_MODLOADER_H
