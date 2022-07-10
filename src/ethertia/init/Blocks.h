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
#include <ethertia/block/BlockLog.h>
#include <ethertia/block/BlockLeaves.h>

class Blocks
{
public:
    inline static std::vector<Block*> REGISTRY;

    static u8 regBlock(Block* bl) {
        Blocks::REGISTRY.push_back(bl);
        return Blocks::REGISTRY.size()-1;
    }

    inline static u8 AIR = regBlock(nullptr);
    inline static u8 STONE = regBlock(new BlockStone());
    inline static u8 DIRT = regBlock(new BlockDirt());
    inline static u8 GRASS = regBlock(new BlockGrass());
    inline static u8 SAND = regBlock(new BlockSand());
    inline static u8 WATER = regBlock(new BlockWater());
    inline static u8 LOG = regBlock(new BlockLog());
    inline static u8 LEAVES = regBlock(new BlockLeaves());

};

#endif //ETHERTIA_BLOCKS_H
