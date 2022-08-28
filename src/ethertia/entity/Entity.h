//
// Created by Dreamtowards on 2022/5/5.
//

#ifndef ETHERTIA_ENTITY_H
#define ETHERTIA_ENTITY_H

#include <glm/vec3.hpp>
#include <btBulletDynamicsCommon.h>

class Entity
{
public:
    std::string name;

    inline static btCollisionShape* EMPTY_SHAPE = new btBoxShape(btVector3(0,0,0));

    btCollisionShape* shape = EMPTY_SHAPE;
    btRigidBody* rigidbody = nullptr;

//    glm::vec3 position;
//    glm::vec3 velocity;

    glm::vec3 prevposition{0};
    glm::vec3 intpposition{0};

    Model* model = nullptr;
    Texture* diffuseMap = Texture::UNIT;

    Entity(float mass = 10) {

        if (mass) {
            VertexBuffer* vbuf = Loader::loadOBJ_("entity/cone.obj");
            model = Loader::loadModel(vbuf);
            shape = createHullShape(vbuf->vertexCount(), vbuf->positions.data());
        }

        initRigidbody(mass);
    }
    ~Entity() {

        delete rigidbody->getMotionState();
        delete rigidbody;

        if (shape != EMPTY_SHAPE)
            delete shape;
    }

    void updateShape(btCollisionShape* s) {

        rigidbody->getLocalInertia();
        rigidbody->setCollisionShape(s);
    }

    void initRigidbody(float mass)
    {
        btVector3 localInertia(0, 0, 0);
        if (mass) {
            shape->calculateLocalInertia(mass, localInertia);
        }

        rigidbody = new btRigidBody(mass, new btDefaultMotionState(), shape, localInertia);
        rigidbody->setUserPointer(this);
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

    void setPosition(glm::vec3 p) {
        rigidbody->getWorldTransform().setOrigin(btVector3(p.x, p.y, p.z));
    }
    glm::vec3 getPosition() {
        btVector3& p = rigidbody->getWorldTransform().getOrigin();
        return glm::vec3(p.x(), p.y(), p.z());
    }

    glm::mat3 getRotation() {
        btMatrix3x3& basis = rigidbody->getWorldTransform().getBasis();
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
            rigidbody->setActivationState(ACTIVE_TAG);
        }

        auto o = rigidbody->getLinearVelocity();
        rigidbody->setLinearVelocity(btVector3(o.x()+vel.x, o.y()+vel.y, o.z()+vel.z));
    }


};

#endif //ETHERTIA_ENTITY_H
