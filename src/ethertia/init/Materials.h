//
// Created by Dreamtowards on 2022/9/11.
//

#ifndef ETHERTIA_MATERIALS_H
#define ETHERTIA_MATERIALS_H

#include <ethertia/world/Cell.h>

#include <ethertia/material/Material.h>

class Materials
{
public:

#define REGISTER_MTL(VAR, id) inline static Material* VAR = new Material{id};

    // 用0/nullptr 表示空气/空. 因为实在是太方便了 if(c.mtl)。而且目前没什么副作用
    inline static Material* AIR = nullptr;

    REGISTER_MTL(STONE, "rock");
    REGISTER_MTL(GRASS, "grass");
    REGISTER_MTL(MOSS, "moss");
    REGISTER_MTL(DIRT, "black_dirt");
    REGISTER_MTL(SAND, "sand");
    REGISTER_MTL(LOG, "oak_log");
    REGISTER_MTL(PLANKS, "plank");
    REGISTER_MTL(LEAVES, "leaves");
    REGISTER_MTL(TALLGRASS, "tallgrass");
    REGISTER_MTL(WATER, "water");




};

#endif //ETHERTIA_MATERIALS_H
