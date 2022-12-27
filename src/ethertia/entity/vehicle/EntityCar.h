//
// Created by Dreamtowards on 2022/9/2.
//

#ifndef ETHERTIA_ENTITYCAR_H
#define ETHERTIA_ENTITYCAR_H

#include <vector>

#include "Entity.h"

class EntityCar : public Entity
{
public:

    std::vector<btTypedConstraint*> constraints;
    std::vector<btRigidBody*> rigidbodies;

    EntityCar()
    {
//        loadModelAndShape("entity/bulletcar.obj", &m_Model);
//
//        init();
    }

    void onLoad(btDynamicsWorld *dworld) override {
        for (btTypedConstraint* c : constraints) {
            dworld->addConstraint(c, true);
        }
        for (btRigidBody* r : rigidbodies) {
            dworld->addRigidBody(r);
        }
    }

    void onUnload(btDynamicsWorld *dworld) override {
        for (btTypedConstraint* c : constraints) {
            dworld->removeConstraint(c);
        }
        for (btRigidBody* r : rigidbodies) {
            dworld->removeRigidBody(r);
        }
    }

//    void init() {
//
//        // BODY
//        btCompoundShape* compound = new btCompoundShape();
//        {
//            btCollisionShape* chassisShape = new btBoxShape(btVector3(1.f, 0.5f, 2.f));
//            compound->addChildShape(Mth::btTransf({0, 1, 0}), chassisShape);
//
//            btCollisionShape* suppShape = new btBoxShape(btVector3(0.5f, 0.1f, 0.5f));
//            compound->addChildShape(Mth::btTransf({0, 1.0, 2.5}), suppShape);
//        }
//
//        const btScalar chassisMass = 2.0f;
//        const btScalar wheelMass = 1.0f;   //m_carChassis->setDamping(0.2,0.2);
//        const btScalar FALLHEIGHT = 5;
//        btRigidBody* m_carChassis = newRigidbody(chassisMass, compound, Mth::btTransf({0, FALLHEIGHT, 0}));
//        rigidbodies.push_back(m_carChassis);
//
//        rigidbody = m_carChassis;
//
//        // WHEELs
//        glm::vec3 wheelPos[4] = {
//                {-1., FALLHEIGHT-0.25, 1.25},
//                {1., FALLHEIGHT-0.25, 1.25},
//                {1., FALLHEIGHT-0.25, -1.25},
//                {-1., FALLHEIGHT-0.25, -1.25}};
//
//        static float wheelRadius = 0.5f;
//        static float wheelWidth = 0.4f;   //m_wheelShape = new btCylinderShapeX(btVector3(wheelWidth,wheelRadius,wheelRadius));
//        btCollisionShape* m_wheelShape = new btCylinderShapeX(btVector3(wheelWidth, wheelRadius, wheelRadius));
//
//        for (int i = 0; i < 4; i++)
//        {
//            btRigidBody* pBodyA = m_carChassis;
//            pBodyA->setActivationState(DISABLE_DEACTIVATION);
//
//            btRigidBody* pBodyB = newRigidbody(wheelMass, m_wheelShape, Mth::btTransf(wheelPos[i]));
//            rigidbodies.push_back(pBodyB);
//            pBodyB->setFriction(1110);
//            pBodyB->setActivationState(DISABLE_DEACTIVATION);
//            // add some data to build constraint frames
//            btVector3 parentAxis(0.f, 1.f, 0.f);
//            btVector3 childAxis(1.f, 0.f, 0.f);
//            btVector3 anchor = Mth::btVec3(wheelPos[i]);
//            btHinge2Constraint* pHinge2 = new btHinge2Constraint(*pBodyA, *pBodyB, anchor, parentAxis, childAxis);
//            constraints.push_back(pHinge2);
//
//
//            //pHinge2->setLowerLimit(-SIMD_HALF_PI * 0.5f);
//            //pHinge2->setUpperLimit(SIMD_HALF_PI * 0.5f);
//
//            // add constraint to world
//            constraints.push_back(pHinge2);
//
//            // Drive engine.
//            pHinge2->enableMotor(3, true);
//            pHinge2->setMaxMotorForce(3, 1000);
//            pHinge2->setTargetVelocity(3, 0);
//
//            // Steering engine.
//            pHinge2->enableMotor(5, true);
//            pHinge2->setMaxMotorForce(5, 1000);
//            pHinge2->setTargetVelocity(5, 0);
//
//            pHinge2->setParam( BT_CONSTRAINT_CFM, 0.15f, 2 );
//            pHinge2->setParam( BT_CONSTRAINT_ERP, 0.35f, 2 );
//
//            pHinge2->setDamping( 2, 2.0 );
//            pHinge2->setStiffness( 2, 40.0 );
//
//            // pHinge2->setDbgDrawSize(btScalar(5.f));
//        }
//    }

};

#endif //ETHERTIA_ENTITYCAR_H
