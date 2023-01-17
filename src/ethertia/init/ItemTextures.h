//
// Created by Dreamtowards on 2023/1/16.
//

#ifndef ETHERTIA_ITEMTEXTURES_H
#define ETHERTIA_ITEMTEXTURES_H

#include <ethertia/item/Item.h>
#include <ethertia/render/Texture.h>
#include <ethertia/init/Items.h>

class ItemTextures
{
public:
    inline static const int ITEM_RESOLUTION = 128;

    inline static Texture* ITEM_ATLAS = nullptr;

    static Texture* makeAtlas(const std::string& cache_file) {
        if (Loader::fileExists(cache_file)) {
            std::cout << "loading from cache '" << cache_file << "'";
            return Loader::loadTexture(cache_file, false);
        } else {
            std::cout << "cache not found. start baking.";

            int n = Item::REGISTRY.size();

            BitmapImage atlas(ITEM_RESOLUTION*n, ITEM_RESOLUTION);
            BitmapImage resized(ITEM_RESOLUTION, ITEM_RESOLUTION);

            for (int i = 0;i < n;++i)
            {
                Item* item = Item::REGISTRY.m_Ordered[i];
                std::string id = item->getRegistryId();

                std::string loc;
                if (item->hasComponent<ItemComponentMaterial>()) {
                    loc = Strings::fmt("material/{}/view.png", id);
                } else {
                    loc = Strings::fmt("item/{}/view.png", id);
                }

                if (Loader::fileExists(Loader::fileAssets(loc)))
                {
                    BitmapImage img = Loader::loadPNG(Loader::loadAssets(loc));

                    BitmapImage::resizeTo(img, resized);

                    atlas.setPixels(i*ITEM_RESOLUTION, 0, resized);
                }
                else
                {
                    std::cerr  << Strings::fmt("missing item texture '{}'.", loc);
                }
            }

            Loader::savePNG(atlas, cache_file);

            return Loader::loadTexture(atlas);
        }
    }

    static void load()
    {
        BENCHMARK_TIMER;
        Log::info("Loading {} item textures... (x{}) \1", Item::REGISTRY.size(), ITEM_RESOLUTION);


        ITEM_ATLAS = makeAtlas("./cache/item.png");
        
    }

};

#endif //ETHERTIA_ITEMTEXTURES_H
