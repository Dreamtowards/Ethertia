//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_RENDERENGINE_H
#define ETHERTIA_RENDERENGINE_H

#include <glm/glm.hpp>

#include <vkx.h>

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



    inline static vkx::Image* TEX_WHITE = nullptr;
    inline static vkx::Image* TEX_UVMAP = nullptr;

    inline static VkImageView g_ComposeView = nullptr;
};

#endif //ETHERTIA_RENDERENGINE_H
