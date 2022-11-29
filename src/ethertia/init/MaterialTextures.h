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

#include <fmt/core.h>
#include <map>

#include <ethertia/render/TextureAtlas.h>
#include <ethertia/init/Materials.h>

/*
 * We will put all Material-Textures into an Atlas Texture (x horizontal linear extending)
 * Every atlas share same Range
 * one atlas for each texture-type. (5 types now, Diffuse/Displacement/Normal/AO/Roughness)
 *
 * Every material texture has a Range to index these Atlas.
 *
 * every material texture are same resolution.
 * the atlas is X direction (horizontal) extending.
 *
 * 每个 Material 都对应有一个 MaterialTexture (真的吗 会不会有一个Material有多个 tex 比如)
 *
 * 这里将把所有 Material-Textures 放入一系列 Atlas Texture. 每种类型的材质一张 Atlas
 * 每个 Material-Texture 都有一个 Range 去访问这些 Atlas. 每个 Material-Texture 的 Range 在这些 Atlas 中都是相等的.
 */
class MaterialTextures
{
public:

    inline static TextureAtlas* ATLAS_DIFFUSE = new TextureAtlas();
    inline static TextureAtlas* ATLAS_DISP = new TextureAtlas();  // Displacement Maps.
    inline static TextureAtlas* ATLAS_NORM = new TextureAtlas();
    inline static TextureAtlas* ATLAS_AO = new TextureAtlas();
    inline static TextureAtlas* ATLAS_ROUGH = new TextureAtlas();

    static TextureAtlas::Region* load(const char* mtl) {
        const int px = 1024;
        TextureAtlas::Region* r =
        ATLAS_DIFFUSE->addAtlas(Loader::loadPNG(Loader::loadAssets(fmt::format("/materials/{}/diff-{}.png", mtl, px))));
        ATLAS_DISP   ->addAtlas(Loader::loadPNG(Loader::loadAssets(fmt::format("/materials/{}/disp-{}.png", mtl, px))));
        ATLAS_NORM   ->addAtlas(Loader::loadPNG(Loader::loadAssets(fmt::format("/materials/{}/norm-{}.png", mtl, px))));
        return r;
    }

    inline static std::map<std::string, TextureAtlas::Region*> TEXTURES = {
            {"stone", nullptr},
            {"grass", nullptr}
    };

    inline static TextureAtlas::Region* STONE;
    inline static TextureAtlas::Region* GRASS;
    inline static TextureAtlas::Region* DIRT;
    inline static TextureAtlas::Region* SAND;

    static TextureAtlas::Region* of(u8 id) {
        switch (id) {
            case Materials::STONE: return STONE;
            case Materials::GRASS: return GRASS;
            case Materials::DIRT:  return DIRT;
            case Materials::SAND:  return SAND;
            default: throw std::runtime_error("illegal material id "+std::to_string((int)id));
        }
    }

    static void initAtlasRanges() {

        int n = TEXTURES.size();
        float w = 1.0f / n;
        for (int i = 0; i < n; ++i) {

            ATLAS_DIFFUSE->addBakedAtlas(glm::vec2((float)i / (float)n, 0.0f), glm::vec2(w, 1.0f));
        }

    }

    static void loadAtlas() {



    }

    static void makeAtlas(int resolution = 0) {

        int idx = std::distance(TEXTURES.begin(), TEXTURES.find("stone"));



        Loader::savePNG(Texture::glfGetTexImage(ATLAS_DIFFUSE->atlasTexture), "atlas_diff.png");
        Loader::savePNG(Texture::glfGetTexImage(ATLAS_DISP->atlasTexture), "atlas_disp.png");
        Loader::savePNG(Texture::glfGetTexImage(ATLAS_NORM->atlasTexture), "atlas_norm.png");
    }

    static void init()
    {
        BenchmarkTimer tm;
        Log::info("Material Textures loading...\1");

        // we mainly hold one Master Texture (1024px),
        // but for fast load (e.g. for debug, low-performance-device), we provide low resolution textures
        // which make loading way more faster.
        // so given a resolution, we check is that file exists, if not we read master-tex and resize



        STONE = load("gravel");
        GRASS = load("moss");
        DIRT  = //load("dirt_podzol");
        SAND  = //load("sand");
                STONE;

        ATLAS_DIFFUSE->buildAtlas();
        ATLAS_DISP->buildAtlas();
        ATLAS_NORM->buildAtlas();

    }
};

#endif //ETHERTIA_MATERIALTEXTURES_H
