//
// Created by Thaumstrial on 2023-02-01.
//

#ifndef ETHERTIA_ENTITYHELICOPTER_H
#define ETHERTIA_ENTITYHELICOPTER_H

#include <ethertia/entity/vehicle/EntityVehicle.h>

class EntityHelicopter : public EntityVehicle
{
    float normalDeltaForce = 10000;
    float pitchRadianDelta = 0.008;
    float yawRadianDelta = 0.008;
    float rollRadianDelta = 0.008;
public:
    EntityHelicopter() {

        static VertexBuffer* M_Helicopter = Loader::loadOBJ("@material/helicopter/mesh.obj");
        static Model* M_Hel = Loader::loadModel(M_Helicopter);

        m_Model = M_Hel;
        initRigidbody(1000, createHullShape(M_Hel->vertexCount, M_Helicopter->positions.data()));
        m_Rigidbody->setFriction(0.8);
    }

    void move(bool up, bool down, bool front, bool back, bool left, bool right, bool rollLeft, bool rollRight) override {
        btTransform& transform = m_Rigidbody->getWorldTransform();
        btMatrix3x3 basis = transform.getBasis();
        btVector3 forward = basis.getColumn(0).normalized();
        btVector3 normal = basis.getColumn(1).normalized();
        btVector3 radial = basis.getColumn(2).normalized();

        if (up) normalDeltaForce += 100;
        if (down) normalDeltaForce -= 100;
        m_Rigidbody->applyCentralForce(normal * normalDeltaForce);

        float deltaYaw = yawRadianDelta * (front ? -1.0f : (back ? 1.0f : 0));
        float deltaPitch = pitchRadianDelta * (left ? 1.0f : (right ? -1.0f : 0));
        float deltaRoll = rollRadianDelta * (rollLeft ? -1.0f : (rollRight ? 1.0f : 0));
        btQuaternion rotationYaw(radial, deltaYaw);
        btQuaternion rotationPitch(normal, deltaPitch);
        btQuaternion rotationRoll(forward, deltaRoll);
        transform.setRotation(transform.getRotation() * rotationYaw * rotationPitch * rotationRoll);
    }
};

#endif //ETHERTIA_ENTITYHELICOPTER_H
