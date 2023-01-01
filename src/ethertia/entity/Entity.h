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

#include <ethertia/render/Model.h>
#include <ethertia/render/Texture.h>
#include <ethertia/render/VertexBuffer.h>
#include <ethertia/util/Loader.h>

class Entity
{
public:
    btRigidBody* m_Rigidbody = nullptr;

    Model* m_Model = nullptr;
    Texture* m_DiffuseMap = nullptr;

    std::uint32_t m_TypeTag = 0;

    World* m_World;

    class TypeTag {
    public:
        static const uint32_t T_CHUNK_TERRAIN = 16,
                              T_CHUNK_VEGETABLE = 17,
                              T_PLAYER = 32;

        static bool isTerrain(uint32_t t) {
            return t==T_CHUNK_TERRAIN || t==T_CHUNK_VEGETABLE;
        }
    };

    Entity() {
    }


//    glm::vec3 position;
//    glm::vec3 velocity;
//    glm::vec3 prevposition{0};
//    glm::vec3 intpposition{0};

//    static void loadModelAndShape(const std::string& path, Model** md, btCollisionShape** sp = nullptr) {
//        VertexBuffer* vbuf = Loader::loadOBJ(Loader::loadAssetsStr(path));
//        *md = Loader::loadModel(vbuf);
//        if (sp) {
//            *sp = createHullShape(vbuf->vertexCount(), vbuf->positions.data());
//        }
//    }
//    // temp test.
//    Entity(float mass, std::string mpath) {
//
//        btCollisionShape* sp;
//        loadModelAndShape(mpath, &m_Model, &sp);
//        rigidbody = newRigidbody(mass, sp);
//    }
//    // entity types which doesn't need be store. e.g. Chunk Model Proxy, Players
//    static bool isSynthesis(Entity* e) {
//
//        return false;
//    }
    ~Entity() {

        delete m_Rigidbody->getCollisionShape();

        delete m_Rigidbody->getMotionState();
        delete m_Rigidbody;
    }


    virtual void onLoad(btDynamicsWorld* dworld) {
        dworld->addRigidBody(m_Rigidbody);
    }

    virtual void onUnload(btDynamicsWorld* dworld) {
        dworld->removeRigidBody(m_Rigidbody);
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

    void setPosition(glm::vec3 p) {
        m_Rigidbody->getWorldTransform().setOrigin(btVector3(p.x, p.y, p.z));
    }
    glm::vec3 getPosition() {
        btVector3& p = m_Rigidbody->getWorldTransform().getOrigin();
        return glm::vec3(p.x(), p.y(), p.z());
    }

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
//        if (glm::length2(vel)) {
//            rigidbody->setActivationState(ACTIVE_TAG);
//        }

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
    }

    void initRigidbodyConvexModel(float mass, VertexBuffer&& vbuf) {

        m_Model = Loader::loadModel(&vbuf);

        initRigidbody(mass, createHullShape(vbuf.vertexCount(), vbuf.positions.data()));
    }

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
