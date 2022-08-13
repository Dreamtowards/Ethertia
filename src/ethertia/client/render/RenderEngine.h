//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_RENDERENGINE_H
#define ETHERTIA_RENDERENGINE_H

#include <ethertia/world/World.h>
#include <ethertia/client/render/renderer/ChunkRenderer.h>
#include <ethertia/client/render/renderer/gui/GuiRenderer.h>
#include <ethertia/client/render/renderer/gui/FontRenderer.h>
#include <ethertia/util/Frustum.h>
#include <ethertia/client/render/renderer/SkyGradientRenderer.h>

class RenderEngine {

public:
    ChunkRenderer chunkRenderer;
    GuiRenderer guiRenderer;
    FontRenderer fontRenderer;

    SkyGradientRenderer skyGradientRenderer;

    glm::mat4 projectionMatrix{1};
    glm::mat4 viewMatrix{1};

    Frustum viewFrustum{};

    float fov = 90;

    RenderEngine() {

        Log::info("RenderEngine initialized. GL_I: {} | {}, {}", glGetString(GL_VERSION), glGetString(GL_RENDERER), glGetString(GL_VENDOR));
    }

    void updateProjectionMatrix(float ratio_wdh) {
        projectionMatrix = glm::perspective(Mth::radians(fov), ratio_wdh, 0.1f, 1000.0f);
    }

    void updateViewFrustum() {
        viewFrustum.set(projectionMatrix * viewMatrix);
    }

    void renderWorld(World* world)
    {
        glClearColor(0.6*.9, 0.7*.9, 0.8*.9, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        // glEnable(GL_CULL_FACE);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        skyGradientRenderer.render();

        for (auto it : world->getLoadedChunks()) {
            Chunk* chunk = it.second;
            if (!chunk)  // Log::info("NO RENDER Chunk: NULL.");
                continue;
            if (!chunk->model)
                continue;

            // Frustum Culling
            if (!viewFrustum.intersects(chunk->getAABB()))
                continue;

            // Rendering Call.
            chunkRenderer.render(chunk);
        }

        RenderEngine::checkGlError();
    }

    static void checkGlError() {
        uint err = glGetError();
        if (err) {
            Log::warn("###### GL Error ######");
            Log::warn("ERR: {}", err);
        }
    }
};

#endif //ETHERTIA_RENDERENGINE_H
