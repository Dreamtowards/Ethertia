//
// Created by Dreamtowards on 2022/12/17.
//

#ifndef ETHERTIA_SETTINGS_H
#define ETHERTIA_SETTINGS_H

#include <string>
#include <vector>

class Settings
{
public:
    inline static const char* SETTINGS_FILE = "./settings.json";


    inline static int displayWidth  = 1280,
                      displayHeight = 720;

    inline static std::vector<std::string> MODS;


    static void loadSettings();

    static void saveSettings();

    //static void loadProgramArguments(int argc, const char* argv[]) {}



    // DEBUG States

    inline static bool dbgPolyLine = false;
    inline static bool dbgTextInf = false;
    inline static int dbgEntitiesRendered = 0;




};

#endif //ETHERTIA_SETTINGS_H
