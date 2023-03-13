//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_RENDERENGINE_H
#define ETHERTIA_RENDERENGINE_H

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ethertia/render/Camera.h>
#include <ethertia/render/Model.h>
#include <ethertia/util/DtorCaller.h>
#include <ethertia/render/Framebuffer.h>
#include <ethertia/util/Loader.h>
#include <ethertia/util/Colors.h>
#include <ethertia/util/Frustum.h>
#include <ethertia/util/Mth.h>
#include <ethertia/util/Log.h>

class RenderEngine
{
public:
    RenderEngine() = delete;

    static void init();
    static void deinit();

    static void framePrepare();

    static void renderWorld(World* world);

    static void renderWorldGeometry(World* world);

    static void renderWorldCompose(World* world);

    static void checkGlError(std::string_view phase = "") {

    }

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
