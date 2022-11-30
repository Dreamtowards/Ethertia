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

#include <stb/stb_image_resize.h>

#include <ethertia/render/TextureAtlas.h>
#include <ethertia/init/Materials.h>


class MaterialTextures
{
public:

    inline static Texture* ATLAS_DIFFUSE = nullptr;
    inline static Texture* ATLAS_DISP    = nullptr;  // Displacement Maps.
    inline static Texture* ATLAS_NORM    = nullptr;
    inline static Texture* ATLAS_AO      = nullptr;
    inline static Texture* ATLAS_ROUGH   = nullptr;

    inline static std::vector<std::string> TEXTURES = {
            "moss",
            "concrete",
            "gravel"
    };


//    static TextureAtlas::Region* load(const char* mtl) {
//        const int px = 1024;
//        TextureAtlas::Region* r =
//        ATLAS_DIFFUSE->addAtlas(Loader::loadPNG(Loader::loadAssets(fmt::format("/materials/{}/diff-{}.png", mtl, px))));
//        ATLAS_DISP   ->addAtlas(Loader::loadPNG(Loader::loadAssets(fmt::format("/materials/{}/disp-{}.png", mtl, px))));
//        ATLAS_NORM   ->addAtlas(Loader::loadPNG(Loader::loadAssets(fmt::format("/materials/{}/norm-{}.png", mtl, px))));
//        return r;
//    }


//    inline static TextureAtlas::Region* STONE;
//    inline static TextureAtlas::Region* GRASS;
//    inline static TextureAtlas::Region* DIRT;
//    inline static TextureAtlas::Region* SAND;
//
//    static TextureAtlas::Region* of(u8 id) {
//        switch (id) {
//            case Materials::STONE: return STONE;
//            case Materials::GRASS: return GRASS;
//            case Materials::DIRT:  return DIRT;
//            case Materials::SAND:  return SAND;
//            default: throw std::runtime_error("illegal material id "+std::to_string((int)id));
//        }
//    }


    /**
     * @param textype could be: diff/disp/norm/ao/rough
     * @param px needed resolution in pixel
     */
    static Texture* makeAtlas(std::string_view textype, int px, const std::string& cache_file) { BenchmarkTimer _tm;
        BitmapImage* atlas;

        if (Loader::fileExists(cache_file))
        {
            Log::info(" *{} load cached atlas from '{}'.\1", textype, cache_file);
            atlas = Loader::loadPNG(Loader::loadFile(cache_file));
        }
        else
        {
            Log::info(" *{} generate new atlas to '{}'.\1", textype, cache_file);
            const int n = TEXTURES.size();
            atlas = new BitmapImage(px * n, px);

            BitmapImage* tmp = new BitmapImage(px, px);  // resized img.
            for (int i = 0; i < n; ++i)
            {
                BitmapImage* src = Loader::loadPNG(Loader::loadAssets(fmt::format("/materials/{}/{}.png", TEXTURES[i], textype)));

                stbir_resize_uint8((unsigned char*)src->getPixels(), src->getWidth(), src->getHeight(), 0,
                                   (unsigned char*)tmp->getPixels(), tmp->getWidth(), tmp->getHeight(), 0, 4);

                atlas->setPixels(i * px, 0, tmp);
            }
            delete tmp;

            Loader::savePNG(atlas, cache_file);
        }

        return Loader::loadTexture(atlas);
    }

    static void init()
    {
        BenchmarkTimer tm;
        Log::info("Material Textures loading...");


        ATLAS_DIFFUSE = makeAtlas("diff", 256, "caches/atlas_diff.png");
        ATLAS_NORM    = makeAtlas("norm", 256, "caches/atlas_norm.png");
        ATLAS_DISP    = makeAtlas("disp", 256, "caches/atlas_disp.png");

        Log::info("Material Texture Atlases all loaded/generated.\1");
    }
};

#endif //ETHERTIA_MATERIALTEXTURES_H
