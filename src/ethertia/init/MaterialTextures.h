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

#include <map>

#include <stb/stb_image_resize.h>

#include <ethertia/material/Materials.h>


class MaterialTextures
{
public:
    // 1024 elaborated
    // 512  pretty realistic
    // 256  pretty
    // 128  non-realistic
    inline static int TEX_RESOLUTION = 256;

    inline static vkx::Image* ATLAS_DIFFUSE = nullptr;
    inline static vkx::Image* ATLAS_NORM    = nullptr;
    inline static vkx::Image* ATLAS_DRAM    = nullptr;  // DRAM: Disp, Rough, AO, Metal


    static void load()
    {
        BenchmarkTimer tm;
        Log::info("Loading {} material texture/atlases... (x{})", Material::REGISTRY.size(), TEX_RESOLUTION);

        int i = 0;
        for (auto& it : Material::REGISTRY) {
            it.second->m_TexId = i;
            ++i;
        }


        ATLAS_DIFFUSE = makeAtlas("diff", TEX_RESOLUTION, "./cache/atlas_diff.png");
        ATLAS_NORM    = makeAtlas("norm", TEX_RESOLUTION, "./cache/atlas_norm.png");
        ATLAS_DRAM    = makeAtlas("dram", TEX_RESOLUTION, "./cache/atlas_dram.png", true);


        Log::info("Material Texture Atlases all loaded/generated.\1");
    }

    static void clean()
    {
        delete ATLAS_DIFFUSE;
        delete ATLAS_NORM;
        delete ATLAS_DRAM;
    }


    // 我在思考 Mtl 不一定要等于/对应一个 MtlTex
    // 我们可以有很多 Mtl 的轮廓 各种装饰模型 复用MtlTex

    static bool loadMtlTex_ResizeTo(std::string_view mtl, std::string_view textype, BitmapImage& dst_resized)
    {
        std::string asset = Strings::fmt("material/{}/{}.png", mtl, textype);
        std::string path = Loader::fileAssets(asset);
        if (!path.empty()) {
            BitmapImage src = Loader::loadPNG_(path.c_str());
            BitmapImage::resize(src, dst_resized);
            return true;
        } else {
            std::cerr << Strings::fmt(" missed '{}'.", asset);

            //      if (textype == "diff")  resized->fillPixels(0xFFFFFFFF);  // unit 1
            // else if (textype == "disp")  resized->fillPixels(0x808080FF);  // middle height
            // else if (textype == "norm")  resized->fillPixels(0x0000FFFF);  // z=1
            // else if (textype == "rough") resized->fillPixels(0xFFFFFFFF);  // not specular
            return false;
        }
    }

    /**
     * @param textype could be: diff/disp/norm/ao/rough
     * @param px needed resolution in pixel
     */
    static vkx::Image* makeAtlas(std::string_view textype, int px, const std::string& cache_file, bool composeDRAM = false) {
        BENCHMARK_TIMER;

        if (Loader::fileExists(cache_file))
        {
            vkx::Image* t = Loader::loadTexture(cache_file);
            Log::info(" *{} loaded cached atlas from '{}'. (x{})\1", textype, cache_file, t->height);
            return t;
        }
        else
        {
            Log::info(" *{} generate new atlas to '{}'.\1", textype, cache_file);
            const int n = Material::REGISTRY.size();
            BitmapImage atlas(px * n, px);

            if (composeDRAM)
            {
                BitmapImage resized_dram[2] = {  // 0: final resized composed DRAM
                        BitmapImage(px, px),  // Disp
                        BitmapImage(px, px)   // Rough
                };

                int i = 0;
                for (auto& it : Material::REGISTRY)
                {
                    const std::string& id = it.first;

                    loadMtlTex_ResizeTo(id, "disp", resized_dram[0]);
                    loadMtlTex_ResizeTo(id, "rough", resized_dram[1]);
                    // BitmapImage img_ao = loadMtlTex(texName, "ao");
                    // BitmapImage img_metal = loadMtlTex(texName, "metal");

                    BitmapImage& composed = resized_dram[0];  // reuse memory.

                    // Copy/Merge Channel 1.
                    BitmapImage::CopyPixels(0,0,resized_dram[1],
                                            0,0,composed, 1);

                    // Produce to the Atlas.
                    BitmapImage::CopyPixels(0,0, composed,
                                            i*px, 0, atlas);
                    ++i;
                }
            }
            else
            {
                BitmapImage resized(px, px);  // resized img.

                int i = 0;
                for (auto& it : Material::REGISTRY)
                {
                    const std::string& id = it.first;

                    if (loadMtlTex_ResizeTo(id, textype, resized))
                    {
                        BitmapImage::CopyPixels(0,0,resized,
                                                i*px, 0, atlas);
                    }
                    ++i;
                }
            }

            // save cached atlas.
            Loader::savePNG(cache_file, atlas);

            return Loader::loadTexture(atlas);
        }
    }

    static glm::vec2 uv_add(int mtlId) {
        return {(float)mtlId / Material::REGISTRY.size(), 0.0f};
    }
    static glm::vec2 uv_mul() {
        return {1.0f / Material::REGISTRY.size(), 1.0f};
    }

    static glm::vec2 uv_to_atlas_region(glm::vec2 uv, int mtlId) {
        float cap = Material::REGISTRY.size();
        return {
            (float)mtlId/cap + uv.x/cap,
            uv.y
        };
    }

};

#endif //ETHERTIA_MATERIALTEXTURES_H
