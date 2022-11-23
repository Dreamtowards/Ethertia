//
// Created by Dreamtowards on 2022/9/11.
//
// Ethernet
// Etherpedia
// Etheream   Etheream
// Etherald
// Etheria

#ifndef ETHERTIA_MATERIALTEXTURES_H
#define ETHERTIA_MATERIALTEXTURES_H

#include <ethertia/render/TextureAtlas.h>

#include "Materials.h"

class MaterialTextures
{
public:

    inline static TextureAtlas* ATLAS_DIFFUSE = new TextureAtlas();
    inline static TextureAtlas* ATLAS_DISPLACEMENT = new TextureAtlas();

    static TextureAtlas::Region* load(const char* p) {
//        ATLAS_DISPLACEMENT->addAtlas(Loader::loadPNG(Loader::loadAssets(
//                Strings::fmt("/materials/%s_disp.png", p))));
        return ATLAS_DIFFUSE->addAtlas(Loader::loadPNG(Loader::loadAssets(
                Strings::fmt("/materials/{}.png", p))));
    }

    inline static TextureAtlas::Region* STONE = load("stone");
    inline static TextureAtlas::Region* GRASS = load("grass");
    inline static TextureAtlas::Region* DIRT  = load("dirt2");
    inline static TextureAtlas::Region* SAND  = load("sand");

    static TextureAtlas::Region* of(u8 id) {
        switch (id) {
            case Materials::STONE: return STONE;
            case Materials::GRASS: return GRASS;
            case Materials::DIRT:  return DIRT;
            case Materials::SAND:  return SAND;
            default: throw std::runtime_error("illegal material id "+std::to_string((int)id));
        }
    }


    static void init()
    {
        ATLAS_DIFFUSE->buildAtlas();
        ATLAS_DISPLACEMENT->buildAtlas();

#ifndef NDEBUG
        Loader::savePNG(Texture::glfGetTexImage(ATLAS_DIFFUSE->atlasTexture), "atlas_diff.png");
        Loader::savePNG(Texture::glfGetTexImage(ATLAS_DISPLACEMENT->atlasTexture), "atlas_disp.png");
#endif
    }
};

#endif //ETHERTIA_MATERIALTEXTURES_H
