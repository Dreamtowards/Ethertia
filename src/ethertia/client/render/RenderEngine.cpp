//
// Created by Dreamtowards on 2022/8/22.
//

#include "RenderEngine.h"


#include <ethertia/world/World.h>
#include <ethertia/util/Frustum.h>
#include <ethertia/client/render/renderer/ChunkRenderer.h>
#include <ethertia/client/render/renderer/gui/GuiRenderer.h>
#include <ethertia/client/render/renderer/gui/FontRenderer.h>
#include <ethertia/client/render/renderer/SkyGradientRenderer.h>
#include <ethertia/client/render/renderer/EntityRenderer.h>

RenderEngine::RenderEngine()
{
    chunkRenderer = new ChunkRenderer();
    guiRenderer = new GuiRenderer();
    fontRenderer = new FontRenderer();
    entityRenderer = new EntityRenderer();
    skyGradientRenderer = new SkyGradientRenderer();

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
    glm::vec4 _s = Colors::fromRGB(132, 205, 240);  // 0.517, 0.8, 0.94
    glClearColor(_s.x, _s.y, _s.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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


    RenderEngine::checkGlError();
}

void RenderEngine::checkGlError() {
    uint err = glGetError();
    if (err) {
        Log::warn("###### GL Error ######");
        Log::warn("ERR: {}", err);
    }
}