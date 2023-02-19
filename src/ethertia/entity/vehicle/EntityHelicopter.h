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
    // 那个player里的delta是当前帧时间.. 因为帧率可能会波动 不应该影响运动速度 所以*dt



    // CNS. 螺旋桨 Force程度 和质量挂钩..
    float m_EngineForce = 8000;

public:
    EntityHelicopter() {

        static VertexBuffer* M_Helicopter = Loader::loadOBJ("material/helicopter/oka.obj");
        static Model* M_Hel = Loader::loadModel(M_Helicopter);

        m_Model = M_Hel;
        initRigidbody(1000, createHullShape(M_Hel->vertexCount, M_Helicopter->positions.data()));
        m_Rigidbody->setFriction(0.8);

//        btTransform com;
//        com.setOrigin({0, 1, 0});
//        m_Rigidbody->setCenterOfMassTransform(com);
    }

    glm::vec3 to_helicopter_basis_space(glm::vec3 p) {
        return Mth::vec3(m_Rigidbody->getWorldTransform().getBasis() * Mth::btVec3(p));
    }

    // Call from Controls::handleInput().
    void move(bool speedup, bool speeddown,   // 螺旋桨速度
              bool front, bool back, bool left, bool right, // 螺旋桨倾斜方向
              bool rollLeft, bool rollRight) override {  // 尾桨旋转
//        btTransform& transform = m_Rigidbody->getWorldTransform();
//        btMatrix3x3 basis = transform.getBasis();
//        btVector3 forward = basis.getColumn(0).normalized();
//        btVector3 normal = basis.getColumn(1).normalized();
//        btVector3 radial = basis.getColumn(2).normalized();
//
//        if (speedup) normalDeltaForce += 100;
//        if (speeddown) normalDeltaForce -= 100;
//        m_Rigidbody->applyCentralForce(normal * normalDeltaForce);
//
//        float deltaYaw = yawRadianDelta * (front ? -1.0f : (back ? 1.0f : 0));
//        float deltaPitch = pitchRadianDelta * (left ? 1.0f : (right ? -1.0f : 0));
//        float deltaRoll = rollRadianDelta * (rollLeft ? -1.0f : (rollRight ? 1.0f : 0));
//        btQuaternion rotationYaw(radial, deltaYaw);
//        btQuaternion rotationPitch(normal, deltaPitch);
//        btQuaternion rotationRoll(forward, deltaRoll);
//        transform.setRotation(transform.getRotation() * rotationYaw * rotationPitch * rotationRoll);
//
//        return;

        // 假设这里的操控方式是 一根摇杆/轴 代表直升机螺旋桨 （螺旋桨可以倾斜）, 以及尾部的风扇 用来控制旋转方向的
        // 所以 这里 前者会产生一个力 控制线性速度(非角度)，后者会改变旋转朝向yaw. 前者的倾斜方向依赖于后者yaw朝向

        using glm::vec3;

        // Airscrew Axis
        vec3 bias{0, 0, 0};
        if (left)  bias += vec3(-1,0, 0);
        if (right) bias += vec3(1, 0, 0);
        if (front) bias += vec3(0, 0,-1);
        if (back)  bias += vec3(0, 0, 1);

        // apply 'yaw' worldspace.
        bias = to_helicopter_basis_space(bias);

        if (speedup)   bias += vec3(0,  3, 0);
        if (speeddown) bias += vec3(0, -1, 0);  // no possible... 加速下坠



        m_Rigidbody->setActivationState(ACTIVE_TAG);

        m_Rigidbody->applyForce(Mth::btVec3(bias * m_EngineForce),
                                {0, 0.6f, 0});  //螺旋桨位置

        // 模拟质心 回归平衡
        m_Rigidbody->applyForce({0, -2000, 0},
                                Mth::btVec3(to_helicopter_basis_space({0, -5, 0})));

        // Yaw
        vec3 yaw{0};
        if (rollLeft)  yaw += vec3( 1,0,0);  // Rightward Force
        if (rollRight) yaw += vec3(-1,0,0);

        yaw = to_helicopter_basis_space(yaw);

        // apply Yaw
        m_Rigidbody->applyForce(Mth::btVec3(yaw * m_EngineForce),
                                Mth::btVec3(to_helicopter_basis_space({0, 0, 1})));  // 尾浆位置

        // by the way, force set/lock Player View to Vehicle View.
    }
};

#endif //ETHERTIA_ENTITYHELICOPTER_H
