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

    void setMesh(size_t vertCount, float* pos)
    {
        btCollisionShape* oldshape = m_Rigidbody->getCollisionShape();
        delete oldshape;

        if (vertCount) {
            m_Rigidbody->setCollisionShape(createMeshShape(vertCount, pos));
        } else {
            m_Rigidbody->setCollisionShape(new btEmptyShape());
        }
    }

    void setMesh_Model(float* pos, Model* new_model) {
        delete m_Model;

        m_Model = new_model;

        setMesh(new_model->vertexCount, pos);
    }

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
        return new btBvhTriangleMeshShape(mesh, true);
    }

};

#endif //ETHERTIA_ENTITYMESH_H
