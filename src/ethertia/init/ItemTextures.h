//
// Created by Dreamtowards on 2023/1/16.
//

#ifndef ETHERTIA_ITEMTEXTURES_H
#define ETHERTIA_ITEMTEXTURES_H

#include <ethertia/item/Item.h>
#include <ethertia/item/Items.h>



class ItemTextures
{
public:
    inline static const int ITEM_RESOLUTION = 128;

    inline static vkx::Image* ITEM_ATLAS = nullptr;


    static void Load();

    static void Destroy()
    {
        delete ITEM_ATLAS;
    }

};

#endif //ETHERTIA_ITEMTEXTURES_H
