//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_RENDERENGINE_H
#define ETHERTIA_RENDERENGINE_H

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class ChunkRenderer;
class GuiRenderer;
class FontRenderer;
class EntityRenderer;
class SkyGradientRenderer;

class World;

#include <ethertia/util/Frustum.h>
#include <ethertia/util/Mth.h>
#include <ethertia/util/Log.h>

class RenderEngine {

public:
    ChunkRenderer* chunkRenderer             = nullptr;
    GuiRenderer* guiRenderer                 = nullptr;
    FontRenderer* fontRenderer               = nullptr;
    EntityRenderer* entityRenderer           = nullptr;
    SkyGradientRenderer* skyGradientRenderer = nullptr;

    glm::mat4 projectionMatrix{1};
    glm::mat4 viewMatrix{1};

    Frustum viewFrustum{};

    float fov = 90;
    float viewDistance = 3;

    RenderEngine();
    ~RenderEngine();

    void updateProjectionMatrix(float ratio_wdh) {
        projectionMatrix = glm::perspective(Mth::radians(fov), ratio_wdh, 0.1f, 1000.0f);
    }

    void updateViewFrustum() {
        viewFrustum.set(projectionMatrix * viewMatrix);
    }

    void renderWorld(World* world);

    static void checkGlError();
};

#endif //ETHERTIA_RENDERENGINE_H
