//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_RENDERENGINE_H
#define ETHERTIA_RENDERENGINE_H

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class GuiRenderer;
class FontRenderer;
class EntityRenderer;
class SkyGradientRenderer;
class SkyboxRenderer;

class World;

#include <ethertia/render/Camera.h>
#include <ethertia/render/shader/ShaderProgram.h>
#include <ethertia/render/Model.h>
#include <ethertia/render/Framebuffer.h>
#include <ethertia/util/Loader.h>
#include <ethertia/util/Colors.h>
#include <ethertia/util/Frustum.h>
#include <ethertia/util/Mth.h>
#include <ethertia/util/Log.h>

class RenderEngine {

public:
    GuiRenderer* guiRenderer                 = nullptr;
    FontRenderer* fontRenderer               = nullptr;
    EntityRenderer* entityRenderer           = nullptr;
//    SkyGradientRenderer* skyGradientRenderer = nullptr;
//    SkyboxRenderer* skyboxRenderer           = nullptr;

    glm::mat4 projectionMatrix{1};
    glm::mat4 viewMatrix{1};

    Camera m_Camera{};
    Frustum viewFrustum{};

    Framebuffer* gbuffer = nullptr;   // Geometry Buffer FBO, enable MRT (Mutliple Render Targets)
    Framebuffer* dcompose = nullptr;  // Deferred Rendering Compose FBO

    inline static float fov = 90;
    inline static float viewDistance = 1;

    bool debugChunkGeo = false;
    int entitiesActualRendered = 0;

    RenderEngine();
    ~RenderEngine();

    void updateProjectionMatrix(float ratio_wdh) {
        projectionMatrix = glm::perspective(Mth::radians(fov), ratio_wdh, 0.01f, 1000.0f);
    }

    void updateViewFrustum() {
        viewFrustum.set(projectionMatrix * viewMatrix);
    }

    void renderWorld(World* world);

    static void checkGlError(std::string_view phase = "");








    ShaderProgram shaderDebugGeo{Loader::loadAssetsStr("shaders/debug/norm.vsh"),
                                Loader::loadAssetsStr("shaders/debug/norm.fsh"),
                                Loader::loadAssetsStr("shaders/debug/norm.gsh")};

    void renderDebugGeo(Model* model, glm::vec3 pos, glm::mat3 rot) {
        ShaderProgram& shader = shaderDebugGeo;

        shader.useProgram();

        shader.setMatrix4f("matModel", Mth::matModel(pos, rot, glm::vec3(1.0f)));
        shader.setMatrix4f("matView", viewMatrix);
        shader.setMatrix4f("matProjection", projectionMatrix);

        glwDrawArrays(model);
    }


    ShaderProgram shaderDebugBasis{Loader::loadAssetsStr("shaders/debug/model.vsh"),
                                   Loader::loadAssetsStr("shaders/debug/model.fsh")};


    void drawLine(glm::vec3 pos, glm::vec3 dir, glm::vec4 color, bool viewMat = true, bool boxOutline = false) {
        static float M_LINE[]  = {0,0,0,1,1,1};
        static Model* mLine = Loader::loadModel(2, {{3, M_LINE}});
        static float M_BOX[]  = {0,0,0,1,0,0, 1,0,0,1,0,1, 1,0,1,0,0,1, 0,0,1,0,0,0,
                                 0,1,0,1,1,0, 1,1,0,1,1,1, 1,1,1,0,1,1, 0,1,1,0,1,0,
                                 0,0,0,0,1,0, 1,0,0,1,1,0, 1,0,1,1,1,1, 0,0,1,0,1,1,};
        static Model* mBox = Loader::loadModel(24, {{3, M_BOX}});
        Model* model = boxOutline ? mBox : mLine;

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
    void renderLineBox(glm::vec3 min, glm::vec3 size, glm::vec4 color) {
        drawLine(min, size, color, true, true);
    }

    static void glwDrawArrays(Model* model) {
        glBindVertexArray(model->vaoId);
        glDrawArrays(GL_TRIANGLES, 0, model->vertexCount);
    }
};

#endif //ETHERTIA_RENDERENGINE_H
