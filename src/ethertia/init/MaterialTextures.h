//
// Created by Dreamtowards on 2022/9/11.
//
// Ethernet
// Etherpedia
// Etheream   Etheream
// Etherald
// Ether

#ifndef ETHERTIA_MATERIALTEXTURES_H
#define ETHERTIA_MATERIALTEXTURES_H

#include <ethertia/client/render/TextureAtlas.h>

#include "Materials.h"

class MaterialTextures
{
public:

    inline static TextureAtlas* ATLAS = new TextureAtlas();

    static TextureAtlas::Region* load(const char* p) {
        return ATLAS->addAtlas(Loader::loadPNG(Loader::loadAssets(p)));
    }

    inline static TextureAtlas::Region* STONE = load("materials/stone.png");
    inline static TextureAtlas::Region* GRASS = load("materials/grass.png");
    inline static TextureAtlas::Region* DIRT  = load("materials/dirt2.png");
    inline static TextureAtlas::Region* SAND  = load("materials/sand.png");

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

        ATLAS->buildAtlas();
        Loader::savePNG(Texture::glfGetTexImage(ATLAS->atlasTexture), "atlas.png");
    }
};

#endif //ETHERTIA_MATERIALTEXTURES_H
