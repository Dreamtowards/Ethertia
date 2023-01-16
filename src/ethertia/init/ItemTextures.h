//
// Created by Dreamtowards on 2023/1/16.
//

#ifndef ETHERTIA_ITEMTEXTURES_H
#define ETHERTIA_ITEMTEXTURES_H

#include <ethertia/item/Item.h>
#include <ethertia/render/Texture.h>

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
            if (item->hasComponent<Item::ComponentMaterial>()) {
                loc = Strings::fmt("materials/{}/view.png", id);
            } else {
                loc = Strings::fmt("item/{}/view.png", id);
            }
            item->m_CachedTexture = Loader::loadTexture(loc);
        }
    }

};

#endif //ETHERTIA_ITEMTEXTURES_H
