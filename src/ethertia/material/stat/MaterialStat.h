//
// Created by Dreamtowards on 2022/9/11.
//

#ifndef ETHERTIA_MATERIALSTAT_H
#define ETHERTIA_MATERIALSTAT_H

#include <ethertia/util/UnifiedTypes.h>

class MaterialStat
{
public:

    u8 id;
    float density;

    MaterialStat(u8 id = 0, float density = 0.0f) : id(id), density(density) {}

};

#endif //ETHERTIA_MATERIALSTAT_H
