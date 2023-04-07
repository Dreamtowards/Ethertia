//
// Created by Dreamtowards on 2023/1/13.
//

#ifndef ETHERTIA_ITEMSTACK_H
#define ETHERTIA_ITEMSTACK_H

#include <ethertia/item/Item.h>

// It's more like a Slot.
// ItemStack always behaves moving. if add a Stack to another, the source stack will be reduce.

class ItemStack
{
public:

    const Item* m_ItemType = nullptr;

    // Amount or Count ?
    int m_Amount = 0;

    // nbt::tag_compound* m_Data = nullptr;

    ItemStack() {}

    ItemStack(const Item* item, int amount = 1) : m_ItemType(item), m_Amount(amount) {}

    [[nodiscard]] bool empty() const {
        //assert((m_Amount==0) == (m_ItemType== nullptr));
        return m_Amount == 0;
    }

    const Item* item() const {
        return m_ItemType;
    }

    int amount() const {
        return m_Amount;
    }

    void clear() {
        //assert(m_Amount);
        m_ItemType = nullptr;
        m_Amount = 0;
    }

    bool stackableWith(const ItemStack& dst) const {
        assert(dst.m_ItemType);
        return dst.m_ItemType == m_ItemType || (empty() || dst.empty());
    }

    void moveTo(ItemStack& dst, int amount = 0) {
        if (dst.empty()) {
            dst.m_ItemType = m_ItemType;
        }
        assert(stackableWith(dst));

        if (amount == 0) {  // move all.
            dst.m_Amount += m_Amount;
            clear();
        } else if (m_Amount >= amount) {
            dst.m_Amount += amount;
            m_Amount -= amount;
        }
    }
};

#endif //ETHERTIA_ITEMSTACK_H
