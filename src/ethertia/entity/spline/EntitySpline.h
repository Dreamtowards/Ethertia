//
// Created by Dreamtowards on 2022/12/7.
//

#ifndef ETHERTIA_ENTITYSPLINE_H
#define ETHERTIA_ENTITYSPLINE_H

#include <ethertia/entity/Entity.h>

class EntitySpline : public Entity
{
public:

    struct SplinePoint
    {
        glm::vec3 p;
        float angle = 0;
    };

    std::vector<SplinePoint> points;



};

#endif //ETHERTIA_ENTITYSPLINE_H
