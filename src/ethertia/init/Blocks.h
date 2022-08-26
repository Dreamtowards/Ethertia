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

    static u8 regblock(Block* bl) {
        Blocks::REGISTRY.push_back(bl);
        return Blocks::REGISTRY.size()-1;
    }

    inline static u8 AIR = regblock(nullptr);
    inline static u8 STONE = regblock(new BlockStone());
    inline static u8 DIRT = regblock(new BlockDirt());
    inline static u8 GRASS = regblock(new BlockGrass());
    inline static u8 SAND = regblock(new BlockSand());
    inline static u8 WATER = regblock(new BlockWater());
    inline static u8 LOG = regblock(new BlockLog());
    inline static u8 LEAVES = regblock(new BlockLeaves(BlockTextures::LEAVES_2, 1.8f));
    inline static u8 LEAVES_JUNGLE = regblock(new BlockLeaves(BlockTextures::LEAVES_JUNGLE, 1.8f));
    inline static u8 LEAVES_APPLE = regblock(new BlockLeaves(BlockTextures::LEAVES_APPLE, 1.8f));
    inline static u8 TALL_GRASS = regblock(new BlockLeaves(BlockTextures::TALL_GRASS));
    inline static u8 RED_TULIP = regblock(new BlockLeaves(BlockTextures::RED_TULIP));
    inline static u8 SHRUB = regblock(new BlockLeaves(BlockTextures::SHRUB));
    inline static u8 FERN = regblock(new BlockLeaves(BlockTextures::FERN));
    inline static u8 VINE = regblock(new BlockLeaves(BlockTextures::VINE));

};

#endif //ETHERTIA_BLOCKS_H
