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

    inline static u8 AIR   = 0;
    inline static u8 STONE = 1;
    inline static u8 GRASS = 2;
    inline static u8 DIRT  = 3;
    inline static u8 SAND  = 4;


};

#endif //ETHERTIA_MATERIALS_H
