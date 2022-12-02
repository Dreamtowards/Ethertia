//
// Created by Dreamtowards on 2022/8/22.
//

#include "RenderEngine.h"


#include <ethertia/world/World.h>
#include <ethertia/util/Frustum.h>
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

//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//


Framebuffer::gPushFramebuffer(gbuffer);

    glm::vec4 _s = Colors::fromRGB(132, 205, 240);  // 0.517, 0.8, 0.94
    glClearColor(_s.x, _s.y, _s.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glDisable(GL_BLEND);  // Blending is inhabited in Deferred Rendering.

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


    skyboxRenderer->render();


//    glDisable(GL_DEPTH_TEST);
//    glDisable(GL_BLEND);

    entityRenderer->renderCompose(gbuffer->texColor[0], gbuffer->texColor[1], gbuffer->texColor[2]);

    RenderEngine::checkGlError();
}

void RenderEngine::checkGlError() {
    GLuint err = glGetError();
    if (err) {
        Log::warn("###### GL Error ######");
        Log::warn("ERR: {}", err);
    }
}