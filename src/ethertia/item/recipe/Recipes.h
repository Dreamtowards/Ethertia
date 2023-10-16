//
// Created by Dreamtowards on 2023/2/22.
//

#ifndef ETHERTIA_RECIPES_H
#define ETHERTIA_RECIPES_H

#include <ethertia/item/recipe/Recipe.h>
#include <ethertia/item/Items.h>
#include <ethertia/material/Material.h>

class Recipes
{
public:


    static void init()
    {

        // Shears <- Iron Ingot *2
        // Stone Pickaxe <- Stick *2, Stone *3

#define REG_Recipe(result, ...) Recipe::REGISTRY.regist(new Recipe( result , { __VA_ARGS__ }));

        REG_Recipe({Items::SHEARS},
                   {Items::IRON_INGOT, 2});

        REG_Recipe({Items::HOE},
                   {Items::STICK, 2}, {Materials::STONE->item(), 2});

        REG_Recipe(ItemStack(Items::STICK, 8),
                   {Materials::LOG->item()});

        REG_Recipe({Items::PICKAXE},
                   {Items::STICK, 2}, {Materials::STONE->item(), 3});

//        Recipe::REGISTRY.regist(new Recipe(ItemStack(Items::SHEARS, 1),{
//            ItemStack(Items::IRON_INGOT, 1) }));
//        Recipe::REGISTRY.regist(new Recipe(ItemStack(Items::HOE, 1),{
//            ItemStack(Materials::STONE->item(), 2),
//            ItemStack(Items::STICK, 2) } ));
//        Recipe::REGISTRY.regist(new Recipe(Items::STICK,{
//            ItemStack(Materials::LOG->item(), 1) }));

    }

};

#endif //ETHERTIA_RECIPES_H
