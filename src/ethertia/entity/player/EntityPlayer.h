//
// Created by Dreamtowards on 2022/10/6.
//

#ifndef ETHERTIA_ENTITYPLAYER_H
#define ETHERTIA_ENTITYPLAYER_H

#include <ethertia/entity/Entity.h>
#include <ethertia/entity/player/Gamemode.h>
#include <ethertia/init/MaterialMeshes.h>
#include <ethertia/item/Inventory.h>

class EntityPlayer : public Entity
{
    int m_Gamemode = Gamemode::SURVIVAL;

    bool m_Flying = false;

    bool m_Sprint = false;

    EntityVehicle* m_RidingVehicle = nullptr;


public:
    float m_Health = 19.0f;

    bool m_OnGround = false;
    bool m_Riding = false;
    float m_AppliedImpulse = 0;
    int m_NumContactPoints = 0;
    btVector3 m_PrevVelocity{};

    int m_HotbarSlot = 0;

    Inventory m_Inventory{8};


    EntityPlayer() {
        m_TypeTag = Entity::TypeTag::T_PLAYER;

        m_Model = Loader::loadModel(MaterialMeshes::CAPSULE);

        auto* shapeCapsule = new btCapsuleShape(0.5, 2);

        initRigidbody(50.0f, shapeCapsule);

        m_Rigidbody->setAngularFactor(0);  // no rotation
        m_Rigidbody->setSleepingThresholds(0, 0);
        m_Rigidbody->setActivationState(DISABLE_DEACTIVATION);  // no auto deactivate

        m_Rigidbody->setFriction(0.8);
    }

    void setFlying(bool f) {
        m_Flying = f;

        if (f) {
            m_Rigidbody->setDamping(0.96, 0.96);
            m_Rigidbody->setGravity(btVector3(0, 0, 0));
        } else {
            m_Rigidbody->setDamping(0, 0);
            m_Rigidbody->setGravity(btVector3(0, -10, 0));
        }
    }
    bool isFlying() const {
        return m_Flying;
    }

    int getGamemode() {
        return m_Gamemode;
    }

    void switchGamemode(int mode)
    {
        m_Gamemode = mode;

        if (mode == Gamemode::SPECTATOR)
        {
            m_Rigidbody->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);  // no collision

            setFlying(true);
        }
        else
        {
            m_Rigidbody->setCollisionFlags(0);  // resume collision

            if (mode == Gamemode::SURVIVAL) {
                setFlying(false);
            }
        }
    }

    void setSprint(bool s) {
        m_Sprint = s;
    }

    void move(bool up, bool down, bool front, bool back, bool left, bool right)
    {
        if (m_Riding) {

            return;
        }
        float speed = m_Sprint ? 30 : 15;
        speed *= Ethertia::getDelta();

        float yaw = Ethertia::getCamera()->eulerAngles.y;
        glm::vec3 v(0);

        if (m_Flying) {
            speed *= 4;
        } else if (!m_OnGround) {  //falling
            speed *= 0.1f;
        }

        if (front) v += Mth::angleh(yaw) * speed;
        if (back)  v += Mth::angleh(yaw + Mth::PI) * speed;
        if (left)  v += Mth::angleh(yaw + Mth::PI / 2) * speed;
        if (right) v += Mth::angleh(yaw - Mth::PI / 2) * speed;

        if (m_OnGround || m_Flying) {
            float fac = m_OnGround && !m_Flying ? 2.8 : 1.0;
            if (up)   v.y += speed * fac;
            if (down) v.y -= speed * fac;
        }

        applyLinearVelocity(v);
    }

    void enterVehicle(EntityVehicle* pVehicle) {
        m_RidingVehicle = pVehicle;
        m_Riding = true;
    }

    EntityVehicle* exitVehicle() {
        m_Riding = false;
        EntityVehicle* tmp = m_RidingVehicle;
        m_RidingVehicle = nullptr;
        return tmp;
    }

    bool canSeeEntity(glm::vec3 entityVec3, float maxDistance, float maxRadius)
    {
        glm::vec3 playerViewDirection = getViewDirection();
        glm::vec3 playerVec3 = getPosition();
        glm::vec3 viewVec = entityVec3 - playerVec3;
        float distance = glm::length(viewVec);
        if (distance > maxDistance) {
            return false;
        }
        float cosRadius = glm::dot(glm::normalize(viewVec), playerViewDirection);
        if (cosRadius < maxRadius) {
            return false;
        }
        return true;
    }

    ItemStack& getHoldingItem() {
        return m_Inventory.at(m_HotbarSlot);
    }

    glm::vec3 getViewDirection() {
        return Ethertia::getCamera()->direction;
    }

//    class OnGroundCheck : public btCollisionWorld::ContactResultCallback
//    {
//    public:
//        bool m_TestGround = false;
//
//        btScalar addSingleResult(btManifoldPoint &cp,
//                                 const btCollisionObjectWrapper *colObj0, int partId0, int index0,
//                                 const btCollisionObjectWrapper *colObj1, int partId1, int index1) override
//        {
//            btRigidBody* self = Ethertia::getPlayer()->m_Rigidbody;
//
//            if (!m_TestGround && colObj0->getCollisionObject() == self) {
//                btTransform& trans = self->getWorldTransform();
//                btMatrix3x3 invBasis = trans.getBasis().transpose();
//                btVector3 localpoint = invBasis * (cp.m_positionWorldOnB - trans.getOrigin());
//                localpoint.setY(localpoint.getY() + 1.0f);
//
//                if (localpoint.length() < 0.6) {
//                    m_TestGround = true;
//                }
//            }
//
//            return 0;
//        }
//    };

//    void onTick();


};

#endif //ETHERTIA_ENTITYPLAYER_H
