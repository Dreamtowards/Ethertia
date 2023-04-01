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
    btGeneric6DofConstraint* current_constraint = nullptr;

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

        btTransform frameInA, frameInB;
        frameInA.setIdentity();
        frameInB.setIdentity();
        frameInA.setOrigin(btVector3(0.0, 1.5, 0.0));
        frameInB.setOrigin(btVector3(0.0, 0.0, 0.0));

        auto* constraint = new btGeneric6DofConstraint(*m_Rigidbody, *(entityPlayer->m_Rigidbody), frameInA, frameInB, true);

        constraint->setLinearLowerLimit(btVector3(0, 0, 0));
        constraint->setLinearUpperLimit(btVector3(0, 0, 0));
        constraint->setAngularLowerLimit(btVector3(0, 0, 0));
        constraint->setAngularUpperLimit(btVector3(0, 0, 0));

        current_constraint = constraint;

        Ethertia::getWorld()->m_DynamicsWorld->addConstraint(constraint, true);
    }

    // Remove constraint
    void removeDriver() {
        driver = nullptr;
        Ethertia::getWorld()->m_DynamicsWorld->removeConstraint(current_constraint);
        current_constraint = nullptr;
    }
};

#endif //ETHERTIA_ENTITYDRIVINGSEAT_H
