//
// Created by Dreamtowards on 2022/4/29.
//

#ifndef ETHERTIA_COLORS_H
#define ETHERTIA_COLORS_H

#include <glm/vec4.hpp>

class Colors
{
public:
    static const glm::vec4 fromRGB(ubyte r, ubyte g, ubyte b) {
        return glm::vec4(r/255.0f, g/255.0f, b/255.0f, 1.0f);
    }

    inline static constexpr glm::vec4 WHITE = glm::vec4(1, 1, 1, 1);
    inline static constexpr glm::vec4 BLACK = glm::vec4(0, 0, 0, 1);
    inline static constexpr glm::vec4 TRANSPARENT = glm::vec4(0, 0, 0, 0);

    inline static constexpr glm::vec4 WHITE20 = glm::vec4(1, 1, 1, 0.2);
    inline static constexpr glm::vec4 WHITE30 = glm::vec4(1, 1, 1, 0.4);

    inline static constexpr glm::vec4 BLACK80 = glm::vec4(0, 0, 0, 0.8);
    inline static constexpr glm::vec4 BLACK60 = glm::vec4(0, 0, 0, 0.6);
    inline static constexpr glm::vec4 BLACK50 = glm::vec4(0, 0, 0, 0.5);
    inline static constexpr glm::vec4 BLACK40 = glm::vec4(0, 0, 0, 0.4);
    inline static constexpr glm::vec4 BLACK30 = glm::vec4(0, 0, 0, 0.3);
    inline static constexpr glm::vec4 BLACK20 = glm::vec4(0, 0, 0, 0.2);
    inline static constexpr glm::vec4 BLACK10 = glm::vec4(0, 0, 0, 0.1);

    inline static constexpr glm::vec4 R = glm::vec4(1, 0, 0, 1);
    inline static constexpr glm::vec4 G = glm::vec4(0, 1, 0, 1);
    inline static constexpr glm::vec4 B = glm::vec4(0, 0, 1, 1);

    inline static const glm::vec4 RED = fromRGB(255, 85, 85);
    inline static const glm::vec4 RED_DARK = fromRGB(170, 0, 0);
    inline static const glm::vec4 GREEN = fromRGB(85, 255, 85);
    inline static const glm::vec4 GREEN_DARK = fromRGB(0, 170, 0);
    inline static const glm::vec4 BLUE = fromRGB(85, 85, 255);
    inline static const glm::vec4 BLUE_DARK = fromRGB(0, 0, 170);
    inline static const glm::vec4 GRAY = fromRGB(170, 170, 170);
    inline static const glm::vec4 GRAY_DARK = fromRGB(85, 85, 85);
    inline static const glm::vec4 AQUA = fromRGB(85, 255, 255);
    inline static const glm::vec4 AQUA_DARK = fromRGB(0, 170, 170);
    inline static const glm::vec4 PURPLE = fromRGB(255, 85, 255);
    inline static const glm::vec4 PURPLE_DARK = fromRGB(170, 0, 170);
    inline static const glm::vec4 YELLOW = fromRGB(255, 255, 85);
    inline static const glm::vec4 GOLD = fromRGB(255, 170, 0);

};

#endif //ETHERTIA_COLORS_H
