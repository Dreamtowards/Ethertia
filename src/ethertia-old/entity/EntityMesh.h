//
// Created by Dreamtowards on 2022/8/29.
//

#ifndef ETHERTIA_ENTITYMESH_H
#define ETHERTIA_ENTITYMESH_H

#include <ethertia/entity/Entity.h>

class EntityMesh : public Entity
{
public:
    bool m_FaceCulling = true;


    EntityMesh() {

        initRigidbody(0.0f, new btEmptyShape());
    }

    void setNoCollision() {
        m_Rigidbody->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
    }

    void onRender() override;


    void setMesh(btCollisionShape* shap)
    {
        btCollisionShape* oldshape = m_Rigidbody->getCollisionShape();
        delete oldshape;

        // reinsert Rigidbody once setCollisionShape(). or Collision Cache will not be clean properly
        m_World->m_DynamicsWorld->removeRigidBody(m_Rigidbody);

        if (shap) {
            m_Rigidbody->setCollisionShape(shap);
        } else {
            m_Rigidbody->setCollisionShape(new btEmptyShape());
        }

        m_World->m_DynamicsWorld->addRigidBody(m_Rigidbody);
    }

    void updateModel(vkx::VertexBuffer* vtxbuf) {
        delete m_Model;
        m_Model = vtxbuf;
    }

//    void setMesh_Model(float* pos, Model* new_model) {
//        delete m_Model;
//
//        m_Model = new_model;
//
//        setMesh(new_model->vertexCount, pos);
//    }

};

#endif //ETHERTIA_ENTITYMESH_H
