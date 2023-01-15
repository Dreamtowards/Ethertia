//
// Created by Dreamtowards on 2023/1/13.
//

#ifndef ETHERTIA_INVENTORY_H
#define ETHERTIA_INVENTORY_H

#include <vector>

#include <ethertia/item/ItemStack.h>

class Inventory
{
public:

    std::vector<ItemStack> m_ItemStacks;

    Inventory(size_t _size) : m_ItemStacks(_size) {

    }

    ItemStack& at(int i) {
        return m_ItemStacks[i];
    }

    // moving.
    void putItemStack(ItemStack& items) {
        for (ItemStack& slot : m_ItemStacks)
        {
            if (slot.stackableWith(items)) {
                items.moveTo(slot);
            }

            if (items.empty())
                break;
        }
    }

    // length or size? size my confuse with 'bytes' concept. but widely used in std containers.
    size_t size() const {
        return m_ItemStacks.size();
    }

};

#endif //ETHERTIA_INVENTORY_H
