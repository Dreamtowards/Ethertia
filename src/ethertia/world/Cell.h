//
// Created by Dreamtowards on 2022/12/2.
//

#ifndef ETHERTIA_CELL_H
#define ETHERTIA_CELL_H

#include <ethertia/material/Material.h>

class Cell
{
public:

    Material* mtl;
    float density;

    glm::vec3 fp;  // featurepoint cache. in-cell.

    Cell(Material* _mtl = nullptr, float _density = 0.0f) : mtl(_mtl), density(_density) {}

};

#endif //ETHERTIA_CELL_H
