//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_MTH_H
#define ETHERTIA_MTH_H

#include <glm/vec3.hpp>

class Mth
{
public:
    static inline int floor(float v, int u)
    {
        int i = (int)(v / (float)u);
        if (i < 0) return (i-1) * u;
        else return i*u;
    }

    static inline glm::vec3 floor(glm::vec3 v, int u)
    {
        return glm::vec3(floor(v.x, u), floor(v.y, u), floor(v.z, u));
    }
};

#endif //ETHERTIA_MTH_H
