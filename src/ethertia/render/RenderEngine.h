//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_RENDERENGINE_H
#define ETHERTIA_RENDERENGINE_H

#include <glm/glm.hpp>

#include <eldaria/vulkan/vkx.h>

//#include "Texture.h"
//#include "VertexArrays.h"
//#include "Framebuffer.h"
//#include "RenderCommand.h"

#include <ethertia/util/AABB.h>
#include <ethertia/util/Mth.h>

class RenderEngine
{
public:
    RenderEngine() = delete;

    static void init();
    static void deinit();

    static void Render();



    // Sun (Day) or Moon (Night).  Noon -> vec3(0, -1, 0)
    static glm::vec3 SunlightDir(float daytime) {
        return -Mth::rot_dir(std::fmod((daytime-0.25f) * 2*Mth::PI, Mth::PI),
                            glm::vec3(0, 0, 1), glm::vec3(1, 0, 0));
    }

    inline static vkx::Image* TEX_WHITE = nullptr;
    inline static vkx::Image* TEX_UVMAP = nullptr;
};

#endif //ETHERTIA_RENDERENGINE_H
