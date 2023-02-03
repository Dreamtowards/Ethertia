//
// Created by Dreamtowards on 2022/12/2.
//

#ifndef ETHERTIA_CELL_H
#define ETHERTIA_CELL_H

#include <ethertia/material/Material.h>
#include <ethertia/util/Mth.h>

class Cell
{
public:

    const Material* mtl = nullptr;
    float density = 0;

    char exp_meta = 0;  // tmp: 1=block.

    glm::vec3 fp{Mth::Inf};  // featurepoint cache. in-cell.

    Cell(Material* _mtl = nullptr, float _density = 0.0f) : mtl(_mtl), density(_density) {}

    bool isOpaqueCube() {
        return mtl && exp_meta == 1;
    }
};

#endif //ETHERTIA_CELL_H
