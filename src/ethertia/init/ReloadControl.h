//
// Created by Dreamtowards on 2023/1/15.
//

#ifndef ETHERTIA_RELOADCONTROL_H
#define ETHERTIA_RELOADCONTROL_H

#include <unordered_map>

#include <ethertia/util/Collections.h>

#include <ethertia/gui/screen/GuiScreenPause.h>

// A reload-function registry center.

class ReloadControl
{
public:

    inline static std::unordered_map<std::string, std::function<void()>> RELOADERS;

    static std::string regist(const std::string& name, const std::function<void()>& reloader) {
        assert(!Collections::existsKey(RELOADERS, name) && "Already registered.");
        RELOADERS[name] = reloader;
        return name;
    }

    static void reload(const std::string& begins)
    {
        for (auto& it : RELOADERS) {
            if (it.first.find(begins) == 0) {
                it.second();
            }
        }
    }

    static void init() {

        ReloadControl::regist("gui-tex.pause", GuiScreenPause::reloadTexture);  // domain-assets_type.spec

    }


};

#endif //ETHERTIA_RELOADCONTROL_H
