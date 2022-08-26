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
#include <ethertia/client/render/shader/ShaderProgram.h>
#include "Model.h"
#include <ethertia/client/Loader.h>
#include <ethertia/util/Colors.h>

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

    float debugChunkGeo = false;

    RenderEngine();
    ~RenderEngine();

    void updateProjectionMatrix(float ratio_wdh) {
        projectionMatrix = glm::perspective(Mth::radians(fov), ratio_wdh, 0.01f, 1000.0f);
    }

    void updateViewFrustum() {
        viewFrustum.set(projectionMatrix * viewMatrix);
    }

    void renderWorld(World* world);

    static void checkGlError();


    ShaderProgram shaderDebugGeo{Loader::loadAssetsStr("shaders/debug/norm.vsh"),
                                Loader::loadAssetsStr("shaders/debug/norm.fsh"),
                                Loader::loadAssetsStr("shaders/debug/norm.gsh")};

    void renderDebugGeo(Model* model, glm::vec3 pos) {
        ShaderProgram& shader = shaderDebugGeo;

        shader.useProgram();

        shader.setMatrix4f("matModel", Mth::matModel(pos));
        shader.setMatrix4f("matView", viewMatrix);
        shader.setMatrix4f("matProjection", projectionMatrix);

        glwDrawArrays(model);
    }


    ShaderProgram shaderDebugBasis{Loader::loadAssetsStr("shaders/debug/model.vsh"),
                                   Loader::loadAssetsStr("shaders/debug/model.fsh")};


    void drawLine(glm::vec3 pos, glm::vec3 dir, glm::vec4 color, bool viewMat = true) {
        static float M_LINE[]  = {0,0,0,1,1,1};
        static Model* model = Loader::loadModel(2, {{3, M_LINE}});
        ShaderProgram& shader = shaderDebugBasis;

        shader.useProgram();

        shader.setMatrix4f("matView", viewMat ? viewMatrix : glm::mat4(1.0f));
        shader.setMatrix4f("matProjection", projectionMatrix);

        shader.setVector4f("color", color);
        shader.setVector3f("direction", dir);
        shader.setVector3f("position", pos);

        glBindVertexArray(model->vaoId);
        glDrawArrays(GL_LINES, 0, model->vertexCount);
    }
    void renderDebugWorldBasis() {
        int n = 4;
        int ex = 1;
        for (int x = -n; x <= n; ++x) {
            drawLine(glm::vec3(x, 0, -n-ex), glm::vec3(0, 0, 2*n+2*ex), Colors::BLACK30, true);
        }
        for (int z = -n; z <= n; ++z) {
            drawLine(glm::vec3(-n-ex, 0, z), glm::vec3(2*n+2*ex, 0, 0), Colors::BLACK30, true);
        }

        float len = 8.0f;
        drawLine(glm::vec3(0), glm::vec3(len, 0, 0), Colors::R, true);
        drawLine(glm::vec3(0), glm::vec3(0, len, 0), Colors::G, true);
        drawLine(glm::vec3(0), glm::vec3(0, 0, len), Colors::B, true);

    }
    void renderDebugBasis() {
        glm::vec3 p = glm::vec3(0.0f, 0.0f, -1.0f);
        float len = 0.2f;

        drawLine(p, glm::mat3(viewMatrix) * glm::vec3(len, 0, 0), Colors::R, false);
        drawLine(p, glm::mat3(viewMatrix) * glm::vec3(0, len, 0), Colors::G, false);
        drawLine(p, glm::mat3(viewMatrix) * glm::vec3(0, 0, len), Colors::B, false);
    }

    static void glwDrawArrays(Model* model) {
        glBindVertexArray(model->vaoId);
        glDrawArrays(GL_TRIANGLES, 0, model->vertexCount);
    }
};

#endif //ETHERTIA_RENDERENGINE_H
