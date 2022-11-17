//
// Created by Dreamtowards on 2022/8/22.
//

#include "RenderEngine.h"


#include <ethertia/world/World.h>
#include <ethertia/util/Frustum.h>
#include <ethertia/client/render/renderer/ChunkRenderer.h>
#include <ethertia/client/render/renderer/EntityRenderer.h>
#include <ethertia/client/render/renderer/gui/GuiRenderer.h>
#include <ethertia/client/render/renderer/gui/FontRenderer.h>
#include <ethertia/client/render/renderer/SkyGradientRenderer.h>
#include <ethertia/client/render/renderer/SkyboxRenderer.h>

RenderEngine::RenderEngine()
{
    chunkRenderer = new ChunkRenderer();
    guiRenderer = new GuiRenderer();
    fontRenderer = new FontRenderer();
    entityRenderer = new EntityRenderer();
    skyGradientRenderer = new SkyGradientRenderer();
    skyboxRenderer = new SkyboxRenderer();

    float qual = 0.5;
    gbuffer = Framebuffer::glfGenFramebuffer((int)(1280 * qual), (int)(720 * qual));
    Framebuffer::gPushFramebuffer(gbuffer);
        gbuffer->attachColorTexture(0, GL_RGB, GL_RGB);
        gbuffer->attachDepthStencilRenderbuffer();
        gbuffer->checkFramebufferStatus();
    Framebuffer::gPopFramebuffer();


    Log::info("RenderEngine initialized. GL_I: {} | {}, {}", glGetString(GL_VERSION), glGetString(GL_RENDERER), glGetString(GL_VENDOR));
}

RenderEngine::~RenderEngine() {

    delete chunkRenderer;
    delete guiRenderer;
    delete fontRenderer;
    delete entityRenderer;
    delete skyGradientRenderer;
}

void RenderEngine::renderWorld(World* world)
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

Framebuffer::gPushFramebuffer(gbuffer);

    glm::vec4 _s = Colors::fromRGB(132, 205, 240);  // 0.517, 0.8, 0.94
    glClearColor(_s.x, _s.y, _s.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    skyboxRenderer->render();

//    skyGradientRenderer->render();

//    {
//        std::lock_guard<std::mutex> guard(world->chunklock);
//
//        for (auto it : world->getLoadedChunks()) {
//            Chunk* chunk = it.second;
//            if (!chunk)  // Log::info("NO RENDER Chunk: NULL.");
//                continue;
//            if (!chunk->model)
//                continue;
//
//            // Frustum Culling
//            if (!viewFrustum.intersects(chunk->getAABB()))
//                continue;
//
//            // Rendering Call.
//            chunkRenderer->render(chunk);
//
//            if (debugChunkGeo)
//                renderDebugGeo(chunk->model, chunk->position);
//        }
//    }

    entitiesActualRendered = 0;
    for (Entity* entity : world->getEntities()) {
        // frustum test.
        if (!entity)
            continue;
        if (!entity->model)
            continue;

        // Frustum Culling
        if (!viewFrustum.intersects(entity->getAABB())) {
            continue;
        }
        ++entitiesActualRendered;

        entityRenderer->render(entity, chunkRenderer->shader);

        if (debugChunkGeo)
            renderDebugGeo(entity->model, entity->getPosition(), entity->getRotation());
    }
    // Log::info("Rendered: {}/{}", numRdr, world->getEntities().size());


Framebuffer::gPopFramebuffer();

    Gui::drawRect(0, 0, Gui::maxWidth(), Gui::maxHeight(), Colors::WHITE, gbuffer->texColor[0]);


    RenderEngine::checkGlError();
}

void RenderEngine::checkGlError() {
    GLuint err = glGetError();
    if (err) {
        Log::warn("###### GL Error ######");
        Log::warn("ERR: {}", err);
    }
}