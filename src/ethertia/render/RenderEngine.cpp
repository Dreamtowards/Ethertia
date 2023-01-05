//
// Created by Dreamtowards on 2022/8/22.
//

#include "RenderEngine.h"


#include <ethertia/world/World.h>
#include <ethertia/render/renderer/EntityRenderer.h>
#include <ethertia/render/renderer/SkyGradientRenderer.h>
#include <ethertia/render/renderer/SkyboxRenderer.h>
#include <ethertia/render/renderer/gui/GuiRenderer.h>
#include <ethertia/render/renderer/gui/FontRenderer.h>

RenderEngine::RenderEngine()
{
    BenchmarkTimer _tm;
    Log::info("RenderEngine initializing.\1");

    std::cout << " renderers[";
    guiRenderer = new GuiRenderer();      std::cout << "gui, ";
    fontRenderer = new FontRenderer();    std::cout << "font, ";
    entityRenderer = new EntityRenderer();std::cout << "entity";
//    skyGradientRenderer = new SkyGradientRenderer();
//    skyboxRenderer = new SkyboxRenderer();
    std::cout << "]";

    float qual = 0.6;
    gbuffer = Framebuffer::glfGenFramebuffer((int)(1280 * qual), (int)(720 * qual));
    Framebuffer::gPushFramebuffer(gbuffer);
        gbuffer->attachColorTexture(0, GL_RGBA32F, GL_RGBA, GL_FLOAT);      // Positions, Depth, f16 *3
        gbuffer->attachColorTexture(1, GL_RGB32F, GL_RGB, GL_FLOAT);        // Normals,          f16 *3
        gbuffer->attachColorTexture(2, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE); // Albedo,           u8 *3
        gbuffer->setupMRT({0, 1, 2});

        gbuffer->attachDepthStencilRenderbuffer();
        gbuffer->checkFramebufferStatus();
    Framebuffer::gPopFramebuffer();

    dcompose = Framebuffer::glfGenFramebuffer(gbuffer->width, gbuffer->height);
    Framebuffer::gPushFramebuffer(dcompose);
        dcompose->attachColorTexture(0, GL_RGBA);
        dcompose->checkFramebufferStatus();
    Framebuffer::gPopFramebuffer();

    RenderEngine::checkGlError("End of RenderEngine Init");

}

RenderEngine::~RenderEngine() {

    delete guiRenderer;
    delete fontRenderer;
    delete entityRenderer;
//    delete skyGradientRenderer;
//    delete skyboxRenderer;
}

void RenderEngine::renderWorld(World* world)
{
    assert(world);

    // Geometry of Deferred Rendering
    {
        PROFILE("Geo");
Framebuffer::gPushFramebuffer(gbuffer);

    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glDisable(GL_BLEND);  // Blending is inhabited in Deferred Rendering.
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    g_NumEntityRendered = 0;
    for (Entity* entity : world->getEntities()) {
        if (!entity || !entity->m_Model) continue;

        // Frustum Culling
        if (!viewFrustum.intersects(entity->getAABB())) {
            continue;
        }

        if (entity->m_TypeTag == Entity::TypeTag::T_CHUNK_VEGETABLE) {
            if (dbg_NoVegetable) continue;
            glDisable(GL_CULL_FACE);
        }

        PROFILE("E/"+std::to_string(entity->m_TypeTag));

        entityRenderer->renderGeometryChunk(entity->m_Model, entity->getPosition(), entity->getRotation(), entity->m_DiffuseMap);

        if (entity->m_TypeTag == Entity::TypeTag::T_CHUNK_VEGETABLE) {
            glEnable(GL_CULL_FACE);  // setback
        }

        ++g_NumEntityRendered;
        if (debugChunkGeo)
            renderDebugGeo(entity->m_Model, entity->getPosition(), entity->getRotation());
    }

    glEnable(GL_BLEND);

Framebuffer::gPopFramebuffer();
    }



    // Compose of Deferred Rendering

    {
        PROFILE("Cmp");
Framebuffer::gPushFramebuffer(dcompose);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    entityRenderer->renderCompose(gbuffer->texColor[0], gbuffer->texColor[1], gbuffer->texColor[2]);

Framebuffer::gPopFramebuffer();
    }


    // Result.

    Gui::drawRect(0, 0, Gui::maxWidth(), Gui::maxHeight(), Colors::WHITE, dcompose->texColor[0]);

    RenderEngine::checkGlError("End World Render");
}

void RenderEngine::checkGlError(std::string_view phase) {
    GLuint err = glGetError();
    if (err) {
        Log::warn("###### GL Error @{} ######", phase);
        Log::warn("ERR: {}", err);
    }
}


//    Get World Ray from Screen Pixel.
//        Window* _wd = Ethertia::getWindow();
//        RenderEngine* _rde = Ethertia::getRenderEngine();
//        glm::vec3 ray = Mth::worldRay(_wd->getMouseX(), _wd->getMouseY(), _wd->getWidth(), _wd->getHeight(), _rde->projectionMatrix, _rde->viewMatrix);
//
//        Log::info("ray {}", glm::to_string(ray));