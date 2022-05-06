//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_BLOCKS_H
#define ETHERTIA_BLOCKS_H

#include <vector>

#include <ethertia/block/Block.h>
#include <ethertia/block/BlockStone.h>
#include <ethertia/block/BlockDirt.h>
#include <ethertia/block/BlockGrass.h>
#include <ethertia/block/BlockSand.h>
#include <ethertia/block/BlockWater.h>

class Blocks
{
public:
    inline static std::vector<Block*> REGISTRY;

    static ubyte regBlock(Block* bl) {
        Blocks::REGISTRY.push_back(bl);
        return Blocks::REGISTRY.size()-1;
    }

    inline static ubyte AIR = regBlock(nullptr);
    inline static ubyte STONE = regBlock(new BlockStone());
    inline static ubyte DIRT = regBlock(new BlockDirt());
    inline static ubyte GRASS = regBlock(new BlockGrass());
    inline static ubyte SAND = regBlock(new BlockSand());
    inline static ubyte WATER = regBlock(new BlockWater());

};

#endif //ETHERTIA_BLOCKS_H
