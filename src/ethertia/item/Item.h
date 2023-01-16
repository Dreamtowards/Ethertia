//
// Created by Dreamtowards on 2023/1/13.
//

#ifndef ETHERTIA_ITEM_H
#define ETHERTIA_ITEM_H

#include <ethertia/init/Registry.h>

class Item {
public:
    inline static Registry<Item> REGISTRY;

    class Component {
    public:
        virtual void makePoly() {}
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
    bool hasComponent() {
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

    };

    class ComponentMaterial : public Item::Component {
    public:

        int m_Material;

        ComponentMaterial(int mtl) : m_Material(mtl) {}

    };
};

class Items
{
public:

    inline static const Item* PICKAXE = new Item("pickaxe", {});
    inline static const Item* APPLE = new Item("apple", {new Item::ComponentFood(1.5)});
    inline static const Item* LIME = new Item("lime", {new Item::ComponentFood(0.5)});
//    inline static const Item* MELON = new Item("melon", {new Item::ComponentFood(1.0)});

    // CNS 关于 MaterialItems, 应该每个material对应一个新物品吗? 还是所有的 material 都是同一个 Item::MATERIAL + 额外 MtlId 数据?
    // 如果是前者, 那么可自定义性很大 一个mtl又可以摆放 又可以吃 又可以当工具.. 然而这种情况不常见?
    // 如果是后者 那么不会有什么'id污染' 和特例。比如现在加载mtl item的材质时 要特例去读materials的资源 但其实就算后者也要读。
    // 所以目前用前者 毕竟mtl是核心内容。而且还没发现坏处。
    inline static const Item* STONE = new Item("stone", {new Item::ComponentMaterial(1)});

};

#endif //ETHERTIA_ITEM_H
