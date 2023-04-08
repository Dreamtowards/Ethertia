//
// Created by Dreamtowards on 2023/2/22.
//

#ifndef ETHERTIA_ENTITYCHICKEN_H
#define ETHERTIA_ENTITYCHICKEN_H

#include <ethertia/entity/Entity.h>

class EntityChicken : public Entity
{
public:

    float m_Health;  //max: 32

    EntityChicken()
    {

        initRigidbody(1.0f, new btSphereShape(0.2f));
        m_Rigidbody->setGravity({0, -0.8, 0});


    }


};

#endif //ETHERTIA_ENTITYCHICKEN_H
