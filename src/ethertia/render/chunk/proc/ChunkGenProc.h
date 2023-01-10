//
// Created by Dreamtowards on 2023/1/7.
//

#ifndef ETHERTIA_CHUNKGENPROC_H
#define ETHERTIA_CHUNKGENPROC_H

#include <thread>

class ChunkGenProc
{
public:
    using vec3 = glm::vec3;

    inline static int g_Running = 0;

    static void initThread() {
        new std::thread([]()
        {
            while (Ethertia::isRunning())
            {
                if (g_Running == -1 || g_Running == 0) {
                    g_Running = 0;
                    Timer::sleep_for(1);
                    continue;
                }
                World* world = Ethertia::getWorld();

                vec3 chunkpos = findNearestNotLoadedChunk(world, Ethertia::getCamera()->position, RenderEngine::viewDistance);

                if (chunkpos.x != Mth::Inf)
                {
                    // Gen or Load.
                    Chunk* chunk = world->provideChunk(chunkpos);

                    chunk->invalidateMesh();

                    // CNS. 隐患：在密集生成区块时请求更新周围网格，可能造成过多的无用的互相更新。并且这里没有顾及到8个角
                    for (int i = 0; i < 6; ++i) {
                        world->requestRemodel(chunkpos + Mth::QFACES[i] * 16.0f, false);
                    }
                }

                // Unload Chunks
                _UnloadChunks(world, Ethertia::getCamera()->position, RenderEngine::viewDistance);

            }
        });
    }

    static vec3 findNearestNotLoadedChunk(World* world, vec3 center, int viewDistance) {
        vec3 origin_chunkpos = Chunk::chunkpos(center);

        float minDist = FLT_MAX;
        vec3 nil_chunkpos{Mth::Inf};

        ChunkMeshProc::walkViewDistanceChunks(viewDistance, [&](vec3 rel_chunkpos)
        {
            float dist = glm::length2(rel_chunkpos);
            vec3 chunkpos = origin_chunkpos + rel_chunkpos;
            //if (chunkpos.y < -16 || chunkpos.y > 50) return ;
            if (dist < minDist && world->getLoadedChunk(chunkpos) == nullptr) {
                minDist = dist;
                nil_chunkpos = chunkpos;
            }
        });
        return nil_chunkpos;
    }

    static int _UnloadChunks(World* world, vec3 center, int viewDistance) {
        vec3 cpos = Chunk::chunkpos(center);
        int lim = viewDistance * 16;

        std::vector<glm::vec3> unloads;  // separate remove-op from iterate
        for (auto it : world->getLoadedChunks()) {  // todo: Not Lock yet, need lock?
            vec3 p = it.first;
            if (abs(p.x-cpos.x) > lim ||
                abs(p.y-cpos.y) > lim ||
                abs(p.z-cpos.z) > lim) {
                unloads.push_back(p);
            }
        }

        for (glm::vec3 cp : unloads) {
            world->unloadChunk(cp);
        }
        return unloads.size();
    }

};

#endif //ETHERTIA_CHUNKGENPROC_H
