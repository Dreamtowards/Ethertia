//
// Created by Dreamtowards on 2023/2/23.
//

#ifndef ETHERTIA_ENTITYFURNACE_H
#define ETHERTIA_ENTITYFURNACE_H

#include <ethertia/item/ItemStack.h>

class EntityFurnace : public Entity
{
public:

    // furnace current left burn time in seconds.
    float m_BurnTime = 0;

    ItemStack m_Fuel;

    ItemStack m_Smelt;

    ItemStack m_Output;

    EntityFurnace()
    {

        initRigidbody(30.0f, new btSphereShape(0.2f));
    }

//    bool canSmelt() {
//        if (m_Fuel.empty())
//            return false;
//        if (m_Smelt.hasNotResult())
//            return false;
//        if (m_Output.empty())
//            return true;
//        if (!m_Output.isItemEquals(SmeltResult))
//            return false;
//        return true;  // mOutput.stackSize < limit.
//    }
//
//    void smelt() {
//
//    }

};

#endif //ETHERTIA_ENTITYFURNACE_H
