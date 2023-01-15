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

    inline static std::unordered_map<std::string, Texture*> TEXTURES;

    static void load()
    {
        BENCHMARK_TIMER;
        Log::info("Loading item textures...\1");

        for (auto& it : Item::REGISTRY)
        {
            std::string id = it.first;

            TEXTURES[id] = Loader::loadTexture(Strings::fmt("item/{}/view.png", id));
        }
    }

};

#endif //ETHERTIA_ITEMTEXTURES_H
