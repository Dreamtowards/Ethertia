//
// Created by Dreamtowards on 2022/9/11.
//

#ifndef ETHERTIA_MATERIALTEXTURES_H
#define ETHERTIA_MATERIALTEXTURES_H

#include <ethertia/client/render/TextureAtlas.h>

class MaterialTextures
{
public:

    inline static TextureAtlas* ATLAS = new TextureAtlas();

    static TextureAtlas::Region* load(const char* p) {
        return ATLAS->addAtlas(Loader::loadPNG(Loader::loadAssets(p)));
    }

    inline static TextureAtlas::Region* STONE = load("materials/stone.png");
    inline static TextureAtlas::Region* GRASS = load("materials/grass.png");
    inline static TextureAtlas::Region* DIRT  = load("materials/dirt.png");



    static void init()
    {

        ATLAS->buildAtlas();
        Loader::savePNG(Texture::glfGetTexImage(ATLAS->atlasTexture), "atlas.png");
    }
};

#endif //ETHERTIA_MATERIALTEXTURES_H
