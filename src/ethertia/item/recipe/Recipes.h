//
// Created by Dreamtowards on 2023/2/22.
//

#ifndef ETHERTIA_RECIPES_H
#define ETHERTIA_RECIPES_H

#include <ethertia/item/recipe/Recipe.h>
#include <ethertia/item/Items.h>

class Recipes
{
public:

    static void init()
    {

        // Shears <- Iron Ingot *2
        // Stone Pickaxe <- Stick *2, Stone *3
        Recipe::REGISTRY.regist(new Recipe(Items::SHEARS, {Items::STICK}));

    }

};

#endif //ETHERTIA_RECIPES_H
