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

    guiRenderer = new GuiRenderer();
    fontRenderer = new FontRenderer();
    entityRenderer = new EntityRenderer();
    skyGradientRenderer = new SkyGradientRenderer();
    skyboxRenderer = new SkyboxRenderer();

    float qual = 0.8;
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

    RenderEngine::checkGlError("Init End");

}

RenderEngine::~RenderEngine() {

    delete guiRenderer;
    delete fontRenderer;
    delete entityRenderer;
    delete skyGradientRenderer;
    delete skyboxRenderer;
}

void RenderEngine::renderWorld(World* world)
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Geometry of Deferred Rendering

Framebuffer::gPushFramebuffer(gbuffer);

    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glDisable(GL_BLEND);  // Blending is inhabited in Deferred Rendering.
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    entitiesActualRendered = 0;
    for (Entity* entity : world->getEntities()) {
        if (!entity || !entity->model) continue;

        // Frustum Culling
        if (!viewFrustum.intersects(entity->getAABB())) {
            continue;
        }
        ++entitiesActualRendered;

        entityRenderer->renderGeometry(entity);

        if (debugChunkGeo)
            renderDebugGeo(entity->model, entity->getPosition(), entity->getRotation());
    }

    glEnable(GL_BLEND);

Framebuffer::gPopFramebuffer();



    // Compose of Deferred Rendering

Framebuffer::gPushFramebuffer(dcompose);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    skyboxRenderer->render();

    entityRenderer->renderCompose(gbuffer->texColor[0], gbuffer->texColor[1], gbuffer->texColor[2]);

Framebuffer::gPopFramebuffer();


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