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

//    static bool isFaceCulling(Entity* e) {
//        EntityMesh* m = dynamic_cast<EntityMesh*>(e);
//        return m ? m->m_FaceCulling : false;
//    }

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

    void updateModel(Model* model) {
        delete m_Model;
        m_Model = model;
    }

//    void setMesh_Model(float* pos, Model* new_model) {
//        delete m_Model;
//
//        m_Model = new_model;
//
//        setMesh(new_model->vertexCount, pos);
//    }

    static btBvhTriangleMeshShape* createMeshShape(size_t vertexCount, const float* position) {
        btTriangleMesh* mesh = new btTriangleMesh();
        for (int i = 0; i < vertexCount; i += 3) {
            const float* p = &position[i*3];
            mesh->addTriangle(
                    btVector3(p[0], p[1], p[2]),
                    btVector3(p[3], p[4], p[5]),
                    btVector3(p[6], p[7], p[8])
            );
        }
        return new btBvhTriangleMeshShape(mesh, false);
    }

};

#endif //ETHERTIA_ENTITYMESH_H
