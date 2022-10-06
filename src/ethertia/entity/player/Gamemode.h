//
// Created by Dreamtowards on 2022/10/6.
//

#ifndef ETHERTIA_GAMEMODE_H
#define ETHERTIA_GAMEMODE_H

class Gamemode
{
public:

    static const int SURVIVAL  = 1,
                     CREATIVE  = 2,
                     SPECTATOR = 3,
                     ADVENTURE = 4;

    inline static const std::vector<std::string> NAMES = {{
        "survival",
        "creative"
    }};

    static std::string nameOf(int mode) {
        switch (mode) {
            case SURVIVAL:  return "survival";
            case CREATIVE:  return "creative";
            case SPECTATOR: return "spectator";
            case ADVENTURE: return "adventure";
            default: throw std::runtime_error("illegal Gamemode");
        }
    }
};

#endif //ETHERTIA_GAMEMODE_H
