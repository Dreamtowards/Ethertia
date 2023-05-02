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
    btPoint2PointConstraint* current_constraint = nullptr;

public:
    EntityDrivingSeat()
    {

//        m_DiffuseMap = Texture::WHITE;

        static VertexData* M_DrivingSeat = Loader::loadOBJ("material/driving_seat/driving_seat.obj");
        m_Model = Loader::loadVertexBuffer(M_DrivingSeat);
        initRigidbody(1, CreateHullShape(M_DrivingSeat));
        m_Rigidbody->setFriction(0.8);
    }

    // Restrict driver in seat
    void setDriver(EntityPlayer* entityPlayer) {

        // If sit already contains driver, remove.
        if (driver != nullptr)
        {
            removeDriver();
            return;
        }

        driver = entityPlayer;

        btVector3 pivot1 = btVector3(0, 0, 0);
        btVector3 pivot2 = btVector3(0, -1.5, 0);

        auto* constraint = new btPoint2PointConstraint(*m_Rigidbody, *(entityPlayer->m_Rigidbody), pivot1, pivot2);

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
