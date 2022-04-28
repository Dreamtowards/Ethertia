//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_RENDERENGINE_H
#define ETHERTIA_RENDERENGINE_H

#include <ethertia/world/World.h>
#include <ethertia/client/render/renderer/ChunkRenderer.h>

class RenderEngine {

public:
    ChunkRenderer chunkRenderer;

    glm::mat4 projectionMatrix;

    void updateProjectionMatrix(float ratio_wdh) {
        projectionMatrix = glm::perspective(90.0f, ratio_wdh, 0.1f, 1000.0f);
    }


    void renderWorld(World* world) const
    {
        glClearColor(0, 0, 0.4, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        for (auto it : world->getLoadedChunks())
        {
            if (!it.second){
                // Log::info("NO RENDER Chunk: NULL.");
                continue;
            }
            if (!it.second->model)
                continue;
            chunkRenderer.render(it.second);
        }


        RenderEngine::checkGlError();
    }

    static void checkGlError() {
        uint err = glGetError();
        if (err) {
            Log::warn("###### GL Error ######");
            Log::warn("ERR: "+std::to_string(err));
        }
    }
};

#endif //ETHERTIA_RENDERENGINE_H
