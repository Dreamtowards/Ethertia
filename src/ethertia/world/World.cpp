//
// Created by Dreamtowards on 2022/4/22.
//

#include "World.h"
#include <ethertia/client/Ethertia.h>

void World::onTick()
{
    glm::vec3 cpos = Chunk::chunkpos(Ethertia::getCamera()->position);
    int n = 4;

    // load chunks
    for (int dx = -n;dx <= n;dx++) {
        for (int dy = -n;dy <= n;dy++) {
            for (int dz = -n;dz <= n;dz++) {
                provideChunk(cpos + glm::vec3(dx, dy, dz) * 16.0f);
            }
        }
    }
//    provideChunk(glm::vec3(0, 16*0, 0));

    // unload chunks
    int lim = n*Chunk::SIZE;
    std::vector<glm::vec3> unloads;  // separate iterate / remove
    for (auto itr : getLoadedChunks()) {
        glm::vec3 cp = itr.first;
        if (abs(cp.x-cpos.x) > lim || abs(cp.y-cpos.y) > lim || abs(cp.z-cpos.z) > lim) {
            unloads.push_back(cp);
        }
    }
    for (glm::vec3 cp : unloads) {
        unloadChunk(cp);
    }

}