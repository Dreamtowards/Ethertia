//
// Created by Dreamtowards on 2023/1/31.
//

#ifndef ETHERTIA_SHAREDLIBRARYLOADER_H
#define ETHERTIA_SHAREDLIBRARYLOADER_H

#ifdef __APPLE__
#include <dlfcn.h>
#endif

#include <ethertia/util/Log.h>


class SharedlibraryLoader
{
public:
    using InitFunc = void(*)();

    static bool load(const char* file)
    {

#ifdef __APPLE__

        void* handle = dlopen(file, RTLD_LAZY);
        if (!handle) {
            Log::warn("Unable to load sharedlibrary.");
            return false;
        }

        InitFunc func = (InitFunc)dlsym(handle, "init");

        if (!func) {
            Log::warn("Unable to load init proc.");
            return false;
        }

        func();

        dlclose(handle);

        return true;

#elif __WIN32__

        // https://stackoverflow.com/questions/72249298/dynamic-load-a-class-from-a-dll-in-windows

        void* handle = LoadLibrary(file.c_str());
        if (!handle) {
            // error
        }

        InitFunc f = GetProcAddress((HINSTANCE)handle, "init");

        // delete.
        // FreeLibrary((HMODULE)x)
#endif

    }

};

#endif //ETHERTIA_SHAREDLIBRARYLOADER_H
