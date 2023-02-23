//
// Created by Dreamtowards on 2023/1/17.
//

#ifndef ETHERTIA_ITEMS_H
#define ETHERTIA_ITEMS_H


#include <ethertia/item/ItemComponents.h>

#include <ethertia/entity/EntityLantern.h>
#include <ethertia/entity/vehicle/EntityHelicopter.h>
#include <ethertia/entity/EntityTorch.h>

class Items
{
public:

#define REGISTER_ITEM(VAR, id, comps) inline static const Item* VAR = new Item(id, comps);


    // Food
    REGISTER_ITEM(APPLE,        "apple",        {new ItemComponentFood(1.5)});
    REGISTER_ITEM(LIME,         "lime",         {new ItemComponentFood(0.5)});


    // Tool
    REGISTER_ITEM(PICKAXE,      "pickaxe",      {new ItemComponentTool()});
    REGISTER_ITEM(GRAPPLE,      "grapple",      {new ItemComponentToolGrapple()});
    REGISTER_ITEM(HOE,          "hoe",          {new ItemComponentTool()});
    REGISTER_ITEM(SHEARS,       "shears",       {new ItemComponentTool()});

    // Craft Material
    REGISTER_ITEM(STICK,        "stick",        {new ItemComponentFuel(3.0)});
    REGISTER_ITEM(COAL,         "coal",         {new ItemComponentFuel(16.0)});
    REGISTER_ITEM(IRON_INGOT,   "iron_ingot",   {});


    // Lights
    REGISTER_ITEM(LANTERN,      "lantern",      {new ItemComponentEntity{[](){ return new EntityLantern(); }}});
    REGISTER_ITEM(TORCH,        "torch",        {new ItemComponentEntity([](){ return new EntityTorch(); })});


    // ? Vehicle ?
    REGISTER_ITEM(HELICOPTER,   "helicopter",   {new ItemComponentEntity([](){ return new EntityHelicopter(); })});


};



//enum Id_ {
//    // Tools
//    PICKAXE,
//    SWORD,
//    AXE,
//    HOE,
//    SHEAR,
//    BACKPACK,
//
//    // Food, Vegetable, Meat
//    MELON,
//    PUMPKIN,
//    CARROT,
//    POTATO,
//    POTATO_BAKED,
//    APPLE,
//    BREAD,
//    PORKCHOP,
//    PORKCHOP_COOKED,
//    BEEF,
//    BEEF_COOCKED,
//    VINES,
//    LILY_PAD,
//    // Mushroom, Flower
//
//    COAL,
//    IRON_INGOT,
//    GOLD_INGOT,
//
//    BRICKS,
//    FLOWER_POT,
//    PAINTING,
//    SIGN,
//    RAIL,
//    TORCH,
//    LADDER,
//    MINECART,
//    SPAWN_EGG,
//};

#endif //ETHERTIA_ITEMS_H
