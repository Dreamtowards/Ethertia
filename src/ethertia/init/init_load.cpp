//
// Created by Dreamtowards on 9/10/2023.
//


#include <format>
#include <vkx/vkx.hpp>

#include <ethertia/util/Loader.h>
#include <ethertia/util/BenchmarkTimer.h>
#include <ethertia/util/Log.h>



#pragma region MaterialTextures

#include "MaterialTextures.h"


// 我在思考 Mtl 不一定要等于/对应一个 MtlTex
// 我们可以有很多 Mtl 的轮廓 各种装饰模型 复用MtlTex

static bool _MtlTex_LoadResizeTo(const std::string& mtl, std::string_view textype, BitmapImage& dst_resized)
{
    std::string asset = std::format("material/{}/{}.png", mtl, textype);
    std::string path = Loader::FindAsset(asset);
    if (!path.empty()) {
        BitmapImage src = Loader::LoadPNG_(path.c_str());
        BitmapImage::Resize(src, dst_resized);
        return true;
    } else {
        std::cerr << std::format(" missed '{}'.", asset);

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
static vkx::Image* _MtlTex_MakeAtlas(std::string_view textype, int px, const std::string& cache_file, bool isDRAM = false) 
{
    BENCHMARK_TIMER;

    if (Loader::FileExists(cache_file))
    {
        vkx::Image* t = Loader::LoadImage(cache_file);
        Log::info(" *{} loaded cached atlas from '{}'. (x{})\1", textype, cache_file, t->height);
        return t;
    }
    else
    {
        Log::info(" *{} generate new atlas to '{}'.\1", textype, cache_file);
        const int n = Material::REGISTRY.size();
        BitmapImage atlas(px * n, px);

        if (isDRAM)
        {
            BitmapImage resized_dram[2] = {  // 0: final resized composed DRAM
                    BitmapImage(px, px),  // Disp
                    BitmapImage(px, px)   // Rough
            };

            int i = 0;
            for (auto& it : Material::REGISTRY)
            {
                const std::string& id = it.first;

                _MtlTex_LoadResizeTo(id, "disp", resized_dram[0]);
                _MtlTex_LoadResizeTo(id, "rough", resized_dram[1]);
                // BitmapImage img_ao = loadMtlTex(texName, "ao");
                // BitmapImage img_metal = loadMtlTex(texName, "metal");

                BitmapImage& composed = resized_dram[0];  // reuse memory.

                // Copy/Merge Channel 1.
                BitmapImage::CopyPixels(0,0,resized_dram[1],
                                        0,0,composed, -1, -1,
                                        1);

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

                if (_MtlTex_LoadResizeTo(id, textype, resized))
                {
                    BitmapImage::CopyPixels(0,0,resized,
                                            i*px, 0, atlas);
                }
                ++i;
            }
        }

        // save cached atlas.
        Loader::SavePNG(cache_file, atlas);

        return Loader::LoadImage(atlas);
    }
}

void MaterialTextures::Load()
{
    BENCHMARK_TIMER;
    Log::info("Loading {} material texture/atlases... (x{})", Material::REGISTRY.size(), TEX_RESOLUTION);

    int i = 0;
    for (auto& it : Material::REGISTRY) {
        it.second->m_TexId = i;
        ++i;
    }


    ATLAS_DIFFUSE = _MtlTex_MakeAtlas("diff", TEX_RESOLUTION, "./cache/atlas_diff.png");
    ATLAS_NORM    = _MtlTex_MakeAtlas("norm", TEX_RESOLUTION, "./cache/atlas_norm.png");
    ATLAS_DRAM    = _MtlTex_MakeAtlas("dram", TEX_RESOLUTION, "./cache/atlas_dram.png", true);


    Log::info("Material Texture Atlases all loaded/generated.\1");
}


#pragma endregion


#pragma region ItemTextures


#include "ItemTextures.h"

static vkx::Image* ItemTex_MakeAtlas(const std::string& cache_file, int resolution) {

    if (Loader::FileExists(cache_file)) 
    {
        std::cout << "loading from cache '" << cache_file << "'";
        return Loader::LoadImage(cache_file);
    } 
    else 
    {
        assert(false);
        //std::cout << "cache not found. start baking.";
        //
        //int n = Item::REGISTRY.size();
        //
        //BitmapImage atlas(resolution*n, resolution);
        //BitmapImage resized(resolution, resolution);
        //
        //int i = 0;
        //for (auto& it : Item::REGISTRY)
        //{
        //    Item* item = it.second;
        //    const std::string& id = it.first;
        //
        //    std::string loc;
        //    if (item->hasComponent<ItemComponentMaterial>()) {
        //        loc = std::format("material/{}/view.png", id);
        //        if (Loader::FindAsset(loc).empty()) {  // if Not Found
        //            loc = std::format("material/{}/diff.png", id);
        //        }
        //    } else {
        //        loc = std::format("item/{}/view.png", id);
        //    }
        //
        //    if (!Loader::FindAsset(loc).empty())
        //    {
        //        BitmapImage img = Loader::LoadPNG(loc);
        //        BitmapImage::Resize(img, resized);
        //
        //        BitmapImage::CopyPixels(0, 0, resized,
        //                                i*resolution, 0, atlas);
        //    }
        //    else
        //    {
        //        std::cerr  << std::format("missing item texture '{}'.", loc);
        //    }
        //    i++;
        //}
        //
        //Loader::SavePNG(cache_file, atlas);
        //
        //return Loader::LoadImage(atlas);
    }
}

void ItemTextures::Load()
{
    BENCHMARK_TIMER;
    Log::info("Loading {} item textures... (x{}) \1", Item::REGISTRY.size(), ITEM_RESOLUTION);


    ITEM_ATLAS = ItemTex_MakeAtlas("./cache/item.png", ITEM_RESOLUTION);
}


#pragma endregion







#include "MaterialMeshes.h"


void MaterialMeshes::Load()
{
    BENCHMARK_TIMER;
    Log::info("Loading material meshes...\1");

    CAPSULE = Loader::LoadOBJ("entity/capsule-1-2.obj");

    //for (auto& it : Material::REGISTRY)
    //{
    //    const std::string& id = it.first;
    //    Material* mtl = it.second;
    //
    //    if (mtl->m_CustomMesh)
    //    {
    //        mtl->m_CustomMeshData = Loader::LoadOBJ(std::format("material/{}/mesh.obj", id));
    //    }
    //}
}