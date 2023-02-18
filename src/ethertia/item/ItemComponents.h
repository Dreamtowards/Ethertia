//
// Created by Dreamtowards on 2023/2/18.
//

#ifndef ETHERTIA_ITEMCOMPONENTS_H
#define ETHERTIA_ITEMCOMPONENTS_H

#include <ethertia/item/Item.h>
#include <ethertia/material/Material.h>
#include <ethertia/entity/Entity.h>


class ItemComponentFood : public ItemComponent
{
public:
    float m_Heal = 0;

    ItemComponentFood(float heal) : m_Heal(heal) {}

    void onUse() override;

};

class ItemComponentTool : public ItemComponent
{
public:

    void onUse() override {}

};


class ItemComponentMaterial : public ItemComponent
{
public:
    Material* m_Material;

    ItemComponentMaterial(Material* mtl) : m_Material(mtl) {}

    void onUse() override;

};

class ItemComponentEntity : public ItemComponent
{
public:
    std::function<Entity*()> m_NewEntity;

    ItemComponentEntity(const std::function<Entity*()>& mNew) : m_NewEntity(mNew) {}

    void onUse() override;

};

class ItemComponentGrapple : public ItemComponentTool
{
public:

    void onUse() override;

};



#endif //ETHERTIA_ITEMCOMPONENTS_H
