//
// Created by Dreamtowards on 2022/4/29.
//

#ifndef ETHERTIA_COLORS_H
#define ETHERTIA_COLORS_H

#include <type_traits>  // std::endian

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <ethertia/util/Endian.h>

class Colors
{
public:
    static glm::vec4 ofRGBA(int r, int g, int b, int a = 255) {
        return glm::vec4(r/255.0f, g/255.0f, b/255.0f, a/255.0f);
    }
    static glm::vec4 ofRGB(int r, int g, int b) {  // ofRGBA() default argument are not allowed in static init.
        return Colors::ofRGBA(r,g,b,255);
    }

    // assert glm::to_string(Colors::ofRGBA(0x09080706)) should be vec4(0.035294, 0.031373, 0.027451, 0.023529)
    static glm::vec4 ofRGBA(uint32_t rgba) {
        assert(std::endian::native == std::endian::little);
        return Colors::ofRGBA(
                ((rgba >> 24) & 0xFF),
                ((rgba >> 16) & 0xFF),
                ((rgba >>  8) & 0xFF),
                ( rgba        & 0xFF)
        );
    }
    static glm::vec4 ofRGB(uint32_t rgb) {
        assert(std::endian::native == std::endian::little);
        return Colors::ofRGBA(rgb | 0x000000FF);  // May associated with endian ? i.e. rgb | Endian::bigendian((uint32_t)0xFF)
    }
    static glm::vec4 alpha(glm::vec4 c, float a) {
        c.a = a;
        return c;
    }
    static uint32_t intRGBA(glm::vec4 c) {
        assert(std::endian::native == std::endian::little);
        return uint32_t(  // ?wth.. Endian::bigendian(..)
                ((char(c.x * 255.0f)) << 24) |
                ((char(c.y * 255.0f)) << 16) |
                ((char(c.z * 255.0f)) << 8)  |
                (char(c.w * 255.0f))
        );
    }

    static float parseHex2(const char* hex) {
        char s[3] = {
                hex[0],
                hex[1],
                '\0'
        };
        return std::stoul(s, nullptr, 16) / 255.0f;
    }
    static glm::vec3 parseHexRGB(const char* str) {
        return {
            parseHex2(str),
            parseHex2(str+2),
            parseHex2(str+4)
        };
    }

    inline static float luminance(glm::vec3 rgb) {
        return (rgb.x*0.299f + rgb.y*0.587f + rgb.z*0.114f);
    }

    static glm::vec4 brightness(float f) {
        return {f, f, f, 1.0};
    }

    inline static constexpr glm::vec4 WHITE = glm::vec4(1, 1, 1, 1);
    inline static constexpr glm::vec4 BLACK = glm::vec4(0, 0, 0, 1);
    inline static constexpr glm::vec4 TRANSPARENT = glm::vec4(0, 0, 0, 0);

    inline static constexpr glm::vec4 WHITE80 = glm::vec4(1, 1, 1, 0.8);
    inline static constexpr glm::vec4 WHITE60 = glm::vec4(1, 1, 1, 0.6);
    inline static constexpr glm::vec4 WHITE40 = glm::vec4(1, 1, 1, 0.4);
    inline static constexpr glm::vec4 WHITE30 = glm::vec4(1, 1, 1, 0.3);
    inline static constexpr glm::vec4 WHITE20 = glm::vec4(1, 1, 1, 0.2);
    inline static constexpr glm::vec4 WHITE10 = glm::vec4(1, 1, 1, 0.1);

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

    inline static const glm::vec4 RED        = ofRGB(255, 85, 85);
    inline static const glm::vec4 RED_DARK   = ofRGB(170, 0, 0);
    inline static const glm::vec4 GREEN      = ofRGB(85, 255, 85);
    inline static const glm::vec4 GREEN_DARK = ofRGB(0, 170, 0);
    inline static const glm::vec4 BLUE       = ofRGB(85, 85, 255);
    inline static const glm::vec4 BLUE_DARK  = ofRGB(0, 0, 170);
    inline static const glm::vec4 GRAY       = ofRGB(170, 170, 170);
    inline static const glm::vec4 GRAY_DARK  = ofRGB(85, 85, 85);
    inline static const glm::vec4 AQUA       = ofRGB(85, 255, 255);
    inline static const glm::vec4 AQUA_DARK  = ofRGB(0, 170, 170);
    inline static const glm::vec4 PURPLE     = ofRGB(255, 85, 255);
    inline static const glm::vec4 PURPLE_DARK= ofRGB(170, 0, 170);
    inline static const glm::vec4 YELLOW     = ofRGB(255, 255, 85);
    inline static const glm::vec4 GOLD       = ofRGB(255, 170, 0);

};

#endif //ETHERTIA_COLORS_H
