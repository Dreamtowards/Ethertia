//
// Created by Dreamtowards on 2023/1/24.
//

#ifndef ETHERTIA_ENTITYDROPPEDITEM_H
#define ETHERTIA_ENTITYDROPPEDITEM_H

#include <ethertia/entity/Entity.h>
#include <ethertia/item/ItemStack.h>

class EntityDroppedItem : public Entity
{
public:

    ItemStack m_DroppedItem;

    EntityDroppedItem() {

        initRigidbody(1.0f, new btSphereShape(0.2f));
    }

    void onRender() {

    }

};

#endif //ETHERTIA_ENTITYDROPPEDITEM_H
