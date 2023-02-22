//
// Created by Dreamtowards on 2023/2/19.
//

#ifndef ETHERTIA_RECIPE_H
#define ETHERTIA_RECIPE_H

#include <ethertia/init/Registry.h>
#include <ethertia/item/Item.h>

class Recipe
{
public:
    inline static Registry<Recipe> REGISTRY;


    // may use ItemStack?
    Item* m_Produce;
    std::vector<const Item*> m_Source;

    Recipe(Item* result, const std::vector<const Item*>& source) : m_Produce(result), m_Source(source)
    {

    }

    DECL_RegistryId(m_Result->getRegistryId());


};

#endif //ETHERTIA_RECIPE_H
