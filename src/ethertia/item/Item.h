//
// Created by Dreamtowards on 2023/1/13.
//

#ifndef ETHERTIA_ITEM_H
#define ETHERTIA_ITEM_H

class Item
{
public:

    enum Id {
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

};

#endif //ETHERTIA_ITEM_H
