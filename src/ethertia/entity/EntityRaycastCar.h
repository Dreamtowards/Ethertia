//
// Created by Dreamtowards on 2022/9/11.
//

#ifndef ETHERTIA_ENTITYRAYCASTCAR_H
#define ETHERTIA_ENTITYRAYCASTCAR_H

#include "Entity.h"

class EntityRaycastCar : public Entity
{
public:

    btRaycastVehicle* m_vehicle;

    EntityRaycastCar() {


        loadModelAndShape("entity/bulletcar.obj", &model);
    }



    void onRender() {


        btWheelInfo& wheelInfo = m_vehicle->getWheelInfo(0);



    }

    void onLoad(btDynamicsWorld* dworld) override
    {
        btCompoundShape* compound = new btCompoundShape();
        {
            btCollisionShape* chassisShape = new btBoxShape(btVector3(1.f, 0.5f, 2.f));
            compound->addChildShape(Mth::btTransf({0, 1, 0}), chassisShape);

            btCollisionShape* suppShape = new btBoxShape(btVector3(0.5f, 0.1f, 0.5f));
            compound->addChildShape(Mth::btTransf({0, 1.0, 2.5}), suppShape);
        }
        rigidbody = newRigidbody(100.0f, compound);
        dworld->addRigidBody(rigidbody);
        rigidbody->setActivationState(DISABLE_DEACTIVATION);  /// never deactivate the vehicle

        btRaycastVehicle::btVehicleTuning tuning;

        btVehicleRaycaster* vehicleRaycaster = new btDefaultVehicleRaycaster(dworld);
        m_vehicle = new btRaycastVehicle(tuning, rigidbody, vehicleRaycaster);
        dworld->addVehicle(m_vehicle);

//        m_vehicle->setCoordinateSystem(0, 1, 2);

        btVector3 wheelDirectionCS0(0, -1, 0);
        btVector3 wheelAxleCS(-1, 0, 0);
        float wheelRadius = 0.5f;
        float wheelWidth = 0.4f;
        float suspensionRestLength = 1.6f;
        float connectionHeight = 1.2f;
        float CUBE_HALF_EXTENTS = 1;

        btVector3 connectionPointCS0(CUBE_HALF_EXTENTS - (0.3 * wheelWidth), connectionHeight, 2 * CUBE_HALF_EXTENTS - wheelRadius);
        m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, true);

        connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS + (0.3 * wheelWidth), connectionHeight, 2 * CUBE_HALF_EXTENTS - wheelRadius);
        m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, true);

        connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS + (0.3 * wheelWidth), connectionHeight, -2 * CUBE_HALF_EXTENTS + wheelRadius);
        m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning,false);

        connectionPointCS0 = btVector3(CUBE_HALF_EXTENTS - (0.3 * wheelWidth), connectionHeight, -2 * CUBE_HALF_EXTENTS + wheelRadius);
        m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning,false);


//        for (int i = 0; i < m_vehicle->getNumWheels(); i++)
//        {
//            btWheelInfo& wheel = m_vehicle->getWheelInfo(i);
//            wheel.m_suspensionStiffness = suspensionStiffness;
//            wheel.m_wheelsDampingRelaxation = suspensionDamping;
//            wheel.m_wheelsDampingCompression = suspensionCompression;
//            wheel.m_frictionSlip = wheelFriction;
//            wheel.m_rollInfluence = rollInfluence;
//        }
    }



};

#endif //ETHERTIA_ENTITYRAYCASTCAR_H
