//
// Created by Dreamtowards on 2023/1/13.
//

#ifndef ETHERTIA_ITEM_H
#define ETHERTIA_ITEM_H

#include <ethertia/util/Registry.h>

class ItemComponent
{
public:

    // EntityPlayer* player, float hold_time, bool release
    virtual void onUse() {};

};

enum ItemTag
{
    FOLIAGE,
};

// CNS. 现在Item类没有继承，全靠'ECS'组件实现。虽然ecs还不是个成熟的架构方案 但是尝试它，直到它无法达到我们的功能。目前来说 很灵活。
class Item
{
public:
    inline static Registry<Item> REGISTRY;

    // No. later remove. 这个是没必要的，直接把str id硬编码在getRegistryId才是最好的。那是静态内存. 除非用ECS系统
    std::string Id;

    // ECS for Items
    std::vector<ItemComponent*> m_Components;

    // bool m_Stackable = true;

    Item(const std::string& id, std::initializer_list<ItemComponent*> comps) : Id(id), m_Components(comps)
    {
        REGISTRY.regist(this);

    }


    template<typename T>
    bool hasComponent() const {
        for (ItemComponent* comp : m_Components) {
            if (dynamic_cast<T*>(comp) != nullptr)
                return true;
        }
        return false;
    }

};


#endif //ETHERTIA_ITEM_H
