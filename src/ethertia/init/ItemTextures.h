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

    static void load()
    {
        BENCHMARK_TIMER;
        Log::info("Loading {} item textures...\1", Item::REGISTRY.size());

        for (auto& it : Item::REGISTRY)
        {
            std::string id = it.first;
            Item* item = it.second;

            std::string loc;
            if (item->hasComponent<ItemComponentMaterial>()) {
                loc = Strings::fmt("material/{}/view.png", id);
            } else {
                loc = Strings::fmt("item/{}/view.png", id);
            }
            if (Loader::fileExists(Loader::fileAssets(loc))) {
                item->m_CachedTexture = Loader::loadTexture(loc);
            } else {
                Log::warn("missing item texture '{}'.", loc);
                item->m_CachedTexture = GuiRenderer::TEX_WHITE;
            }
        }
    }

};

#endif //ETHERTIA_ITEMTEXTURES_H
