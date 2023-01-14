//
// Created by Dreamtowards on 2023/1/13.
//

#ifndef ETHERTIA_ITEMSTACK_H
#define ETHERTIA_ITEMSTACK_H

#include <ethertia/item/Item.h>

#include <nbt/nbt_tags.h>

class ItemStack
{
public:

    Item::Id m_ItemId;

    int m_Amount;

    nbt::tag_compound* m_Data;

};

#endif //ETHERTIA_ITEMSTACK_H
