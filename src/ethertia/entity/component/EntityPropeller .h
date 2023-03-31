//
// Created by Thaumstrial on 2023-03-30.
//

#ifndef ETHERTIA_ENTITYPROPELLER_H
#define ETHERTIA_ENTITYPROPELLER_H

#include "ethertia/entity/Entity.h"

//A propeller that gives thrust in one direction
class EntityPropeller : public Entity
{
    float m_EngineForce = 1;
    float m_AngularVelocity = 10;

public:
    EntityPropeller()
    {
        static VertexBuffer* M_Propeller = Loader::loadOBJ("material/propeller/propeller.obj");
        static VertexArrays* M_Hel = Loader::loadModel(M_Propeller);

        m_DiffuseMap = Texture::WHITE;

        m_Model = M_Hel;
        initRigidbody(1, createHullShape(M_Hel->vertexCount, M_Propeller->positions.data()));
        m_Rigidbody->setFriction(0.8);
    }

    void rotate()
    {
        // Rotate
        btVector3 axis(0, 1, 0); // y 轴
        m_Rigidbody->setAngularVelocity(m_AngularVelocity * axis);

        // Thrust force
        btVector3 normal = m_Rigidbody->getWorldTransform().getBasis().getColumn(1);
        btVector3 force = m_EngineForce * normal;
        m_Rigidbody->applyCentralImpulse(force);
    }
};

#endif //ETHERTIA_ENTITYPROPELLER_H
