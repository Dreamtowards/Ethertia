//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_RENDERENGINE_H
#define ETHERTIA_RENDERENGINE_H

#include <glm/glm.hpp>

#include "Texture.h"
#include "VertexArrays.h"
#include "Framebuffer.h"
#include "RenderCommand.h"

#include <ethertia/util/AABB.h>
#include <ethertia/util/Mth.h>

class RenderEngine
{
public:
    RenderEngine() = delete;

    static void init();
    static void deinit();

    static void RenderWorld();

    // Sun (Day) or Moon (Night).  Noon -> vec3(0, -1, 0)
    static glm::vec3 SunlightDir(float daytime) {
        return -Mth::rot_dir(std::fmod((daytime-0.25f) * 2*Mth::PI, Mth::PI),
                            glm::vec3(0, 0, 1), glm::vec3(1, 0, 0));
    }


    static void drawLine(glm::vec3 pos, glm::vec3 dir, glm::vec4 color, bool viewMat = true, bool boxOutline = false);

    static void drawLineBox(glm::vec3 min, glm::vec3 size, glm::vec4 color);

    static void drawLineBox(const AABB& aabb, glm::vec4 color);

};

#endif //ETHERTIA_RENDERENGINE_H
