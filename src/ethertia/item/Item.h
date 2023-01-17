//
// Created by Dreamtowards on 2023/1/13.
//

#ifndef ETHERTIA_ITEM_H
#define ETHERTIA_ITEM_H

#include <ethertia/init/Registry.h>

class Texture;  // for World.h/Server.

class Item {
public:
    inline static Registry<Item> REGISTRY;

    class Component {
    public:
        virtual void onUse() {}
    };

    // No. later remove. 这个是没必要的，直接把str id硬编码在getRegistryId才是最好的。那是静态内存. 除非用ECS系统
    std::string m_Name;

    // ECS for Items
    std::vector<Item::Component *> m_Components;

    bool m_Stackable = true;

    // client only.
    Texture* m_CachedTexture = nullptr;

    Item(const std::string &name, std::initializer_list<Item::Component *> comps) : m_Name(name), m_Components(comps)
    {
        REGISTRY.regist(this);

    }

    std::string getRegistryId() const {
        return m_Name;
    }

    template<typename T>
    bool hasComponent() const {
        for (Item::Component* comp : m_Components) {
            if (dynamic_cast<T*>(comp) != nullptr)
                return true;
        }
        return false;
    }


    enum Id_ {
        // Tools
        PICKAXE,
        SWORD,
        AXE,
        HOE,
        SHEAR,
        BACKPACK,

        // Food, Vegetable, Meat
        MELON,
        PUMPKIN,
        CARROT,
        POTATO,
        POTATO_BAKED,
        APPLE,
        BREAD,
        PORKCHOP,
        PORKCHOP_COOKED,
        BEEF,
        BEEF_COOCKED,
        VINES,
        LILY_PAD,
        // Mushroom, Flower

        COAL,
        IRON_INGOT,
        GOLD_INGOT,

        BRICKS,
        FLOWER_POT,
        PAINTING,
        SIGN,
        RAIL,
        TORCH,
        LADDER,
        MINECART,
        SPAWN_EGG,
    };

    class ComponentFood : public Item::Component {
    public:

        float m_Heal = 2;

        ComponentFood(float heal) : m_Heal(heal) {}

        void onUse() override;

    };

    class ComponentTool : public Item::Component {
    public:

        void onUse() override {

        }

    };

};


#endif //ETHERTIA_ITEM_H
