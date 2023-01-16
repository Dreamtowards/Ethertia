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

#include <ethertia/render/TextureAtlas.h>
#include <ethertia/init/Materials.h>


class MaterialTextures
{
public:
    // 1024 elaborated
    // 512  pretty realistic
    // 256  pretty
    // 128  non-realistic
    inline static int TEX_RESOLUTION = 256;

    inline static Texture* ATLAS_DIFFUSE = nullptr;
    inline static Texture* ATLAS_NORM    = nullptr;
    inline static Texture* ATLAS_DRAM    = nullptr;  // DRAM: Disp, Rough, AO, Metal

    inline static Texture* ATLAS_DISP    = nullptr;  // Displacement Maps.
    inline static Texture* ATLAS_ROUGH   = nullptr;



    // 由于每个mtl纹理很大加载很慢 并且atlas是渲染时必须的，所以我们通常用预先烘烤的atlas 而不是每次加载所有mtl纹理再烘烤 那就要太久了.
    // 然而我们需要一个持续的对照表 根据mtl得知其atlas_tex_id，由于 Material::REGISTRY 是无序的且不提供数字id (不是设计缺陷)，
    // 所以我们需要一个*持续*顺序的表 来表示mtl对应的tex_id 用于缓存atlas. tex_id 将会存在 Material*->m_AtlasTexId
    inline static std::vector<Material*> MTL_ORDERED;
//            {
//            "rock",
//            "grass",
//            "moss",
//            "black_dirt",
//            "sand",
//            "oak_log",
//            "plank",
//            "leaves",
//            "tallgrass",
//            "water"
//    };

    static void load()
    {
        BenchmarkTimer tm;
        Log::info("Material Textures loading...");


        // Build Mtl-to-AtlasTexIdx table.
        MTL_ORDERED.reserve(Material::REGISTRY.size());
        for (auto& it : Material::REGISTRY) {
            MTL_ORDERED.push_back(it.second);
        }
        std::sort(MTL_ORDERED.begin(), MTL_ORDERED.end(), [](Material* l, Material* r) {
            return l->getRegistryId() < r->getRegistryId();
        });
        for (int i = 0; i < MTL_ORDERED.size(); ++i) {
            MTL_ORDERED[i]->m_AtlasTexIdx = i;
        }


//        ATLAS_DIFFUSE = tryLoadCachedAtlas("cache/atlas_diff.png");
//        if (!ATLAS_DIFFUSE) {
//            BitmapImage* img = makeAtlas("diff", TEX_RESOLUTION);
//            ATLAS_DIFFUSE = Loader::loadTexture(img);
//            saveCacheAtlas(img);
//            delete img;
//        }

        const int dest_res = TEX_RESOLUTION;
        ATLAS_DIFFUSE = makeAtlas("diff", dest_res, "cache/atlas_diff.png");
        ATLAS_NORM    = makeAtlas("norm", dest_res, "cache/atlas_norm.png");
        ATLAS_DISP    = makeAtlas("disp", dest_res, "cache/atlas_disp.png");
        ATLAS_ROUGH   = makeAtlas("rough",dest_res, "cache/atlas_rough.png");

//        makeDRAM_Atlas();

        Log::info("Material Texture Atlases all loaded/generated.\1");
    }

    static Texture* tryLoadCachedAtlas(const std::string& cache_file) {

        return nullptr;
    }


    /**
     * @param textype could be: diff/disp/norm/ao/rough
     * @param px needed resolution in pixel
     */
    static Texture* makeAtlas(std::string_view textype, int px, const std::string& cache_file) {
        BenchmarkTimer _tm;
        BitmapImage* atlas;

        if (Loader::fileExists(cache_file))
        {
            Log::info(" *{} load cached atlas from '{}'.\1", textype, cache_file);
            atlas = Loader::loadPNG(Loader::loadFile(cache_file));
        }
        else
        {
            Log::info(" *{} generate new atlas to '{}'.\1", textype, cache_file);
            const int n = MTL_ORDERED.size();
            atlas = new BitmapImage(px * n, px);

            BitmapImage* resized = new BitmapImage(px, px);  // resized img.
            for (int i = 0; i < n; ++i)
            {
                std::string path = Loader::assetsFile(Strings::fmt("materials/{}/{}.png", MTL_ORDERED[i]->getRegistryId(), textype));
                if (Loader::fileExists(path)) {
                    BitmapImage* src = Loader::loadPNG(Loader::loadFile(path));

                    stbir_resize_uint8((unsigned char*)src->getPixels(), src->getWidth(), src->getHeight(), 0,
                                       (unsigned char*)resized->getPixels(), resized->getWidth(), resized->getHeight(), 0, 4);
                    delete src;
                } else {
                    std::cerr << Strings::fmt(" missed '{}'.", path);
                    //      if (textype == "diff")  resized->fillPixels(0xFFFFFFFF);  // unit 1
                    // else if (textype == "disp")  resized->fillPixels(0x808080FF);  // middle height
                    // else if (textype == "norm")  resized->fillPixels(0x0000FFFF);  // z=1
                    // else if (textype == "rough") resized->fillPixels(0xFFFFFFFF);  // not specular
                }

                atlas->setPixels(i * px, 0, resized);
            }
            delete resized;

            Loader::savePNG(atlas, cache_file);
        }

        return Loader::loadTexture(atlas);
    }

    static glm::vec2 uv_to_atlas_region(glm::vec2 uv, int mtlId) {
        float cap = MTL_ORDERED.size();
        return {
            mtlId/cap + uv.x/cap,
            uv.y
        };
    }

    static glm::vec2 regionOffset(int mtlId) {
        return glm::vec2((mtlId) / (float)MTL_ORDERED.size(), 0.0f);
    }

    static glm::vec2 regionScale() {
        return glm::vec2(1.0f / MTL_ORDERED.size(), 1.0f);
    }

};

#endif //ETHERTIA_MATERIALTEXTURES_H
