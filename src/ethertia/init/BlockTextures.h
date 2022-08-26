//
// Created by Dreamtowards on 2022/4/26.
//

#ifndef ETHERTIA_BLOCKTEXTURES_H
#define ETHERTIA_BLOCKTEXTURES_H

#include <ethertia/client/render/TextureAtlas.h>

typedef TextureAtlas::AtlasFragment AtlasFrag;

class BlockTextures
{
public:

    static AtlasFrag* load(const char* p) {
        return ATLAS->addAtlas(Loader::loadPNG(Loader::loadAssets(p)));
    }

    inline static TextureAtlas* ATLAS = new TextureAtlas();

    inline static AtlasFrag* STONE = load("blocks/stone.png");
    inline static AtlasFrag* DIRT  = load("blocks/dirt.png");
    inline static AtlasFrag* GRASS = load("blocks/grass.png");
    inline static AtlasFrag* SAND  = load("blocks/sand.png");
    inline static AtlasFrag* WATER = load("blocks/water_still_opaque_blu.png");  //water_still_opaque_blu
    inline static AtlasFrag* LOG   = load("blocks/log.png");
    inline static AtlasFrag* LEAVES= load("blocks/leaves.png");
    inline static AtlasFrag* LEAVES_JUNGLE= load("blocks/leaves_jungle.png");
    inline static AtlasFrag* LEAVES_APPLE= load("blocks/rose_bush_top.png");
    inline static AtlasFrag* GLASS = load("blocks/glass.png");
    inline static AtlasFrag* LEAVES_2 = load("blocks/leaves_3.png");
    inline static AtlasFrag* RED_TULIP = load("blocks/red_tulip.png");;
    inline static AtlasFrag* SHRUB = load("blocks/shrub.png");
    inline static AtlasFrag* FERN  = load("blocks/fern.png");
    inline static AtlasFrag* TALL_GRASS = load("blocks/grass_2.png");
    inline static AtlasFrag* VINE = load("blocks/vine_3d.png");

    static void init()
    {
//        ATLAS = new TextureAtlas();
//
//        STONE = load("blocks/stone.png");
//        DIRT = load("blocks/dirt.png");
//        GRASS = load("blocks/grass.png");
//        SAND = load("blocks/sand.png");
//        WATER = load("blocks/water_still.png");
//        LOG = load("blocks/log.png");
//        LEAVES = load("blocks/leaves.png");
//        GLASS = load("blocks/glass.png");
//        LEAVES_2 = load("blocks/leaves_3.png");

        ATLAS->buildAtlas();
        Loader::savePNG(Texture::glfGetTexImage(ATLAS->atlasTexture), "atlas.png");
    }
};

#endif //ETHERTIA_BLOCKTEXTURES_H
