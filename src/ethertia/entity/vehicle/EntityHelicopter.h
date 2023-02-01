//
// Created by Thaumstrial on 2023-02-01.
//

#ifndef ETHERTIA_ENTITYHELICOPTER_H
#define ETHERTIA_ENTITYHELICOPTER_H

#include <ethertia/entity/vehicle/EntityVehicle.h>

class EntityHelicopter : public EntityVehicle
{
public:
    EntityHelicopter() {

        static VertexBuffer* M_Helicopter = Loader::loadOBJ("@material/helicopter/mesh.obj");
        static Model* M_Hel = Loader::loadModel(M_Helicopter);

        m_Model = M_Hel;
        initRigidbody(0.5, createHullShape(M_Hel->vertexCount, M_Helicopter->positions.data()));

    }
};

#endif //ETHERTIA_ENTITYHELICOPTER_H
