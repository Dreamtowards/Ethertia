//
// Created by Dreamtowards on 2022/5/5.
//

#ifndef ETHERTIA_ENTITY_H
#define ETHERTIA_ENTITY_H

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

#include <ethertia/util/AABB.h>
#include <ethertia/world/World.h>

#include <ethertia/entity/EntityComponent.h>
#include <ethertia/render/Model.h>
#include <ethertia/render/Texture.h>
#include <ethertia/render/VertexBuffer.h>
#include <ethertia/util/Loader.h>

#include <ethertia/render/RenderEngine.h>

class Entity
{
public:
    World* m_World = nullptr;
    entt::entity m_EnttEntityHandle = entt::null;

    btRigidBody* m_Rigidbody = nullptr;

    VertexArrays* m_Model = nullptr;
    Texture* m_DiffuseMap = nullptr;
//    std::vector<EntityComponent*> m_Components;


    Entity() {
    }

    ~Entity() {

        delete m_Rigidbody->getCollisionShape();

        delete m_Rigidbody->getMotionState();
        delete m_Rigidbody;
    }

    template<typename T, typename... Args>
    T& addComponent(Args&&... args) {
        assert(!hasComponent<T>() && "Entity already have the component.");
        T& component = m_World->m_EnttRegistry.emplace<T>(m_EnttEntityHandle, std::forward<Args>(args)...);
        return component;
    }

    template<typename T>
    T& getComponent() {
        assert(hasComponent<T>() && "Entity doesn't have such Component.");
        return m_World->m_EnttRegistry.get<T>(m_EnttEntityHandle);
    }

    // hasAny, hasAll
    template<typename T>
    bool hasComponent() {
        return m_World->m_EnttRegistry.any_of<T>(m_EnttEntityHandle);
    }

    template<typename T>
    void removeComponent() {
        assert(hasComponent<T>() && "Entity doesn't have such Component.");
        m_World->m_EnttRegistry.remove<T>(m_EnttEntityHandle);
    }

    int numComponents() {
        int i = 0;
        for (auto [entity] : m_World->m_EnttRegistry.view<entt::exclude_t<>>().each()) {
            ++i;
        }
        return i;
    }



    virtual void onRender();


    virtual void onLoad(World* world) {
//        world->m_DynamicsWorld->addRigidBody(m_Rigidbody);
    }

    virtual void onUnload(World* world) {
//        world->m_DynamicsWorld->removeRigidBody(m_Rigidbody);
    }

//    void setCollisionShape(btCollisionShape* s) {
//        float mass = m_Rigidbody->getMass();
//
//        btVector3 localInertia(0, 0, 0);
//        if (mass) {
//            s->calculateLocalInertia(mass, localInertia);
//        }
//        m_Rigidbody->setCollisionShape(s);
//    }

    glm::vec3& position() {
        return *(glm::vec3*)&m_Rigidbody->getWorldTransform().getOrigin().x();
    }
//
//    void setPosition(glm::vec3 p) {
//        m_Rigidbody->getWorldTransform().setOrigin(btVector3(p.x, p.y, p.z));
//    }
//    glm::vec3 getPosition() {
//        btVector3& p = m_Rigidbody->getWorldTransform().getOrigin();
//        return glm::vec3(p.x(), p.y(), p.z());
//    }
//
    glm::mat3 getRotation() {
        btMatrix3x3& basis = m_Rigidbody->getWorldTransform().getBasis();
        glm::mat3 m;
        for (int r = 0; r < 3; r++)
        {
            for (int c = 0; c < 3; c++)
            {
                m[c][r] = basis[r][c];
            }
        }
        return m;
    }

    void applyLinearVelocity(glm::vec3 vel) {
        if (glm::length2(vel)) {
            m_Rigidbody->setActivationState(ACTIVE_TAG);
        }
        auto o = m_Rigidbody->getLinearVelocity();
        m_Rigidbody->setLinearVelocity(btVector3(o.x()+vel.x, o.y()+vel.y, o.z()+vel.z));
    }


    AABB getAABB() {
        btVector3 min, max;
        m_Rigidbody->getAabb(min, max);
        return AABB(glm::vec3(min.x(), min.y(), min.z()),
                    glm::vec3(max.x(), max.y(), max.z()));
    }




    void initRigidbody(float mass, btCollisionShape* shape, const btTransform& startTransform = btTransform::getIdentity())
    {
        assert(m_Rigidbody == nullptr);

        btVector3 localInertia(0, 0, 0);
        if (mass) {
            shape->calculateLocalInertia(mass, localInertia);
        }

        m_Rigidbody = new btRigidBody(mass, new btDefaultMotionState(startTransform), shape, localInertia);
        m_Rigidbody->setUserPointer(this);

        m_Rigidbody->setGravity(btVector3(0, -10, 0));
        m_Rigidbody->setDamping(0.86, 0.86);
    }
//
////    void initRigidbodyConvexModel(float mass, VertexBuffer&& vbuf) {
////
////        m_Model = Loader::loadModel(&vbuf);
////
////        initRigidbody(mass, createHullShape(vbuf.vertexCount(), vbuf.positions.data()));
////    }
//
    static btConvexHullShape* createHullShape(size_t vertexCount, const float* position) {
        btConvexHullShape* hull = new btConvexHullShape();
        for (int i = 0; i < vertexCount; ++i) {
            const float* p = &position[i*3];
            hull->addPoint(btVector3(p[0], p[1], p[2]), false);
        }
        hull->recalcLocalAabb();
        return hull;
    }
};

#endif //ETHERTIA_ENTITY_H
