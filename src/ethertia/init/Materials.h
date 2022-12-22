//
// Created by Dreamtowards on 2022/9/11.
//

#ifndef ETHERTIA_MATERIALS_H
#define ETHERTIA_MATERIALS_H

#include <ethertia/world/Cell.h>

class Materials
{
public:

    inline static Cell STAT_EMPTY{};

    static const u8 AIR   = 0;
    static const u8 STONE = 1;
    static const u8 GRASS = 2;
    static const u8 MOSS  = 3;
    static const u8 DIRT  = 4;
    static const u8 SAND  = 5;
    static const u8 LOG   = 6;
    static const u8 PLANK = 7;
    static const u8 LEAVES= 8;
    static const u8 TALLGRASS   = 9;
    static const u8 WATER = 10;



};

#endif //ETHERTIA_MATERIALS_H
