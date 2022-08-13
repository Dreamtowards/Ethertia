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
    inline static TextureAtlas* ATLAS;

    inline static AtlasFrag* STONE;
    inline static AtlasFrag* DIRT;
    inline static AtlasFrag* GRASS;
    inline static AtlasFrag* SAND;
    inline static AtlasFrag* WATER;
    inline static AtlasFrag* LOG;
    inline static AtlasFrag* LEAVES;
    inline static AtlasFrag* GLASS;
    inline static AtlasFrag* LEAVES_2;

    static void init()
    {
        ATLAS = new TextureAtlas();

        STONE = load("blocks/stone.png");
        DIRT = load("blocks/dirt.png");
        GRASS = load("blocks/grass.png");
        SAND = load("blocks/sand.png");
        WATER = load("blocks/water_still.png");
        LOG = load("blocks/log.png");
        LEAVES = load("blocks/leaves.png");
        GLASS = load("blocks/glass.png");
        LEAVES_2 = load("blocks/leaves_3.png");

        ATLAS->buildAtlas();
        Loader::savePNG(Texture::glfGetTexImage(ATLAS->atlasTexture), "atlas.png");
    }

    static AtlasFrag* load(const char* p) {
        return ATLAS->addAtlas(Loader::loadPNG(Loader::loadAssets(p)));
    }
};

#endif //ETHERTIA_BLOCKTEXTURES_H
