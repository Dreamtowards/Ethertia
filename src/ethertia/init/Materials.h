//
// Created by Dreamtowards on 2022/9/11.
//

#ifndef ETHERTIA_MATERIALS_H
#define ETHERTIA_MATERIALS_H

#include <ethertia/material/stat/MaterialStat.h>

class Materials
{
public:

    inline static MaterialStat STAT_EMPTY{};

    static const u8 AIR   = 0;
    static const u8 STONE = 1;
    static const u8 GRASS = 2;
    static const u8 DIRT  = 3;
    static const u8 SAND  = 4;


};

#endif //ETHERTIA_MATERIALS_H
