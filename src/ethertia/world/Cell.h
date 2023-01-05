//
// Created by Dreamtowards on 2022/12/2.
//

#ifndef ETHERTIA_CELL_H
#define ETHERTIA_CELL_H

#include <ethertia/util/UnifiedTypes.h>

class Cell
{
public:

    u8 id;
    float density;

    glm::vec3 fp;  // featurepoint cache. in-cell.

    Cell(u8 _id = 0, float _density = 0.0f) : id(_id), density(_density) {}

};

#endif //ETHERTIA_CELL_H
