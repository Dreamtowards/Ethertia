//
// Created by Dreamtowards on 2022/4/29.
//

#ifndef ETHERTIA_COLORS_H
#define ETHERTIA_COLORS_H

#include <glm/vec4.hpp>

class Colors
{
public:
    inline static glm::vec4 WHITE = glm::vec4(1, 1, 1, 1);
    inline static glm::vec4 BLACK = glm::vec4(0, 0, 0, 1);
    inline static glm::vec4 TRANSPARENT = glm::vec4(0, 0, 0, 0);

    inline static glm::vec4 RED = glm::vec4(1, 0, 0, 1);
    inline static glm::vec4 GREEN = glm::vec4(0, 1, 0, 1);
    inline static glm::vec4 BLUE = glm::vec4(0, 0, 1, 1);

};

#endif //ETHERTIA_COLORS_H
