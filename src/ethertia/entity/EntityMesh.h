//
// Created by Dreamtowards on 2022/8/29.
//

#ifndef ETHERTIA_ENTITYMESH_H
#define ETHERTIA_ENTITYMESH_H

#include <ethertia/entity/Entity.h>

class EntityMesh : public Entity
{
public:
    EntityMesh() : Entity(0.0f) {

    }

    void setMesh(Model* md, float* pos)
    {
        // delete shape;
        if (md->vertexCount) {
            model = md;
            shape = createMeshShape(md->vertexCount, pos);
        } else {
            model = nullptr;
            shape = EMPTY_SHAPE;
        }

        rigidbody->setCollisionShape(shape);
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
