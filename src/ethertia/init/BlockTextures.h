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

    static void init()
    {
        ATLAS = new TextureAtlas();

        STONE = load("blocks/stone.png");
        DIRT = load("blocks/dirt.png");
        GRASS = load("blocks/grass.png");
        SAND = load("blocks/sand.png");

        ATLAS->buildAtlas();
        Loader::savePNG(Texture::glfGetTexImage(ATLAS->atlasTexture), "atlas.png");
    }

    static AtlasFrag* load(const char* p) {
        size_t len;
        char* data = Loader::loadAssets(p, &len);
        return ATLAS->addAtlas(Loader::loadPNG(data, len));
    }
};

#endif //ETHERTIA_BLOCKTEXTURES_H
