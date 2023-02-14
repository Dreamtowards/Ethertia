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
    inline static EntityRenderer* entityRenderer           = nullptr;
    SkyboxRenderer* m_SkyboxRenderer         = nullptr;
    ParticleRenderer* m_ParticleRenderer     = nullptr;
    SkyGradientRenderer* m_SkyGradientRenderer = nullptr;

    inline static glm::mat4 matProjection{1};
    inline static glm::mat4 matView{1};

    inline static Camera g_Camera{};
    inline static Frustum m_ViewFrustum{};

    inline static Framebuffer* fboGbuffer = nullptr;   // Geometry Buffer FBO, enable MRT (Mutliple Render Targets)


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

    void renderWorldGeometry(World* world);

    void renderWorldCompose(World* world);


    static void checkGlError(std::string_view phase = "") {
        GLuint err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            Log::warn("###### GL Error @{} ######", phase);
            Log::warn("ERR: {}", err);
        }
    }


    static void clearRenderBuffer() {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }




    static void drawLine(glm::vec3 pos, glm::vec3 dir, glm::vec4 color, bool viewMat = true, bool boxOutline = false);

    static void drawLineBox(glm::vec3 min, glm::vec3 size, glm::vec4 color);

};

#endif //ETHERTIA_RENDERENGINE_H
