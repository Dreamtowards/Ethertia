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
class ParticleRenderer;

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
    SkyboxRenderer* m_SkyboxRenderer         = nullptr;
    ParticleRenderer* m_ParticleRenderer     = nullptr;
    SkyGradientRenderer* m_SkyGradientRenderer = nullptr;

    inline static glm::mat4 matProjection{1};
    inline static glm::mat4 matView{1};

    Camera m_Camera{};
    Frustum m_ViewFrustum{};

    Framebuffer* gbuffer = nullptr;   // Geometry Buffer FBO, enable MRT (Mutliple Render Targets)
    Framebuffer* dcompose = nullptr;  // Deferred Rendering Compose FBO

    inline static float fov = 90;
    inline static float viewDistance = 1;
    inline static int fpsCap = 60;

    inline static bool dbg_EntityGeo = false;
    inline static bool dbg_RenderedEntityAABB = false;
    inline static int dbg_NumEntityRendered = 0;
    inline static bool dbg_NoVegetable = false;
    inline static bool dbg_HitPointEntityGeo = false;

    RenderEngine();
    ~RenderEngine();

    void updateProjectionMatrix(float ratio_wdh) {
        matProjection = glm::perspective(Mth::radians(fov), ratio_wdh, 0.01f, 1000.0f);
    }

    void updateViewFrustum() {
        m_ViewFrustum.set(matProjection * matView);
    }

    void renderWorld(World* world);

    static void checkGlError(std::string_view phase = "");


    void clearRenderBuffer() {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }




    ShaderProgram shaderDebugGeo = Loader::loadShaderProgram("shaders/debug/norm.{}", true);

    void renderDebugGeo(Model* model, glm::vec3 pos, glm::mat3 rot, float limitLen = 0, glm::vec3 limitPos = {}) {
        ShaderProgram& shader = shaderDebugGeo;

        shader.useProgram();

        shader.setMatrix4f("matModel", Mth::matModel(pos, rot, glm::vec3(1.0f)));
        shader.setViewProjection();

        if (limitLen) {
            shader.setFloat("limitLen", limitLen);
            shader.setVector3f("limitPos", limitPos);
        }

        glwDrawArrays(model);

        if (limitLen) {
            shader.setFloat("limitLen", 0);
        }
    }


    ShaderProgram shaderDebugBasis = Loader::loadShaderProgram("shaders/debug/model.{}");


    void drawLine(glm::vec3 pos, glm::vec3 dir, glm::vec4 color, bool viewMat = true, bool boxOutline = false) {
        static float M_LINE[]  = {0,0,0,1,1,1};
        static Model* mLine = Loader::loadModel(2, {{3, M_LINE}});
        static float M_BOX[]  = {0,0,0,1,0,0, 1,0,0,1,0,1, 1,0,1,0,0,1, 0,0,1,0,0,0,
                                 0,1,0,1,1,0, 1,1,0,1,1,1, 1,1,1,0,1,1, 0,1,1,0,1,0,
                                 0,0,0,0,1,0, 1,0,0,1,1,0, 1,0,1,1,1,1, 0,0,1,0,1,1,};
        static Model* mBox = Loader::loadModel(24, {{3, M_BOX}});
        Model* model = boxOutline ? mBox : mLine;

        shaderDebugBasis.useProgram();

        shaderDebugBasis.setViewProjection(viewMat);

        shaderDebugBasis.setVector4f("color", color);
        shaderDebugBasis.setVector3f("direction", dir);
        shaderDebugBasis.setVector3f("position", pos);

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

        drawLine(p, glm::mat3(matView) * glm::vec3(len, 0, 0), Colors::R, false);
        drawLine(p, glm::mat3(matView) * glm::vec3(0, len, 0), Colors::G, false);
        drawLine(p, glm::mat3(matView) * glm::vec3(0, 0, len), Colors::B, false);
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
