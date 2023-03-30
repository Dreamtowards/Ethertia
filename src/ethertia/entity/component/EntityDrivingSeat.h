//
// Created by Thaumstrial on 2023-03-29.
//

#ifndef ETHERTIA_ENTITYDRIVINGSEAT_H
#define ETHERTIA_ENTITYDRIVINGSEAT_H

#include "ethertia/entity/Entity.h"

// Entity that hold the driver
class EntityDrivingSeat : public Entity
{
    EntityPlayer* driver = nullptr;

public:
    EntityDrivingSeat()
    {
        static VertexBuffer* M_DrivingSeat = Loader::loadOBJ("material/driving_seat/driving_seat.obj");
        static VertexArrays* M_Hel = Loader::loadModel(M_DrivingSeat);

        m_DiffuseMap = Texture::WHITE;

        m_Model = M_Hel;
        initRigidbody(1, createHullShape(M_Hel->vertexCount, M_DrivingSeat->positions.data()));
        m_Rigidbody->setFriction(0.8);
    }

    // Restrict driver in seat
    void setDriver(EntityPlayer* entityPlayer) {
        driver = entityPlayer;

        btTransform transformA = m_Rigidbody->getWorldTransform();
        btTransform transformB = entityPlayer->m_Rigidbody->getWorldTransform();
        btTransform relativeTransform = transformA.inverse() * transformB;
        btTransform frameInA = relativeTransform;
        btTransform frameInB = transformB.inverse() * relativeTransform * transformA;

        auto* constraint = new btGeneric6DofConstraint(*m_Rigidbody, *(entityPlayer->m_Rigidbody), frameInA, frameInB, true);

        constraint->setLinearLowerLimit(btVector3(0, 0, 0));
        constraint->setLinearUpperLimit(btVector3(0, 0, 0));
        constraint->setAngularLowerLimit(btVector3(0, 0, 0));
        constraint->setAngularUpperLimit(btVector3(0, 0, 0));

        Ethertia::getWorld()->m_DynamicsWorld->addConstraint(constraint, true);
    }

    void removeDriver(EntityPlayer* entityPlayer) {
        driver = nullptr;
    }
};

#endif //ETHERTIA_ENTITYDRIVINGSEAT_H
