//
// Created by Dreamtowards on 2023/2/19.
//

#ifndef ETHERTIA_RECIPE_H
#define ETHERTIA_RECIPE_H

#include <ethertia/util/Registry.h>
#include <ethertia/item/ItemStack.h>

class Recipe
{
public:
    inline static Registry<Recipe> REGISTRY;


    // may use ItemStack?
    ItemStack m_Result;
    std::vector<ItemStack> m_Source;

    Recipe(ItemStack result, const std::initializer_list<ItemStack>& source) : m_Result(result), m_Source(source)
    {

    }


};

#endif //ETHERTIA_RECIPE_H
