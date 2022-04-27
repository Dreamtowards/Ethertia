//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_WORLD_H
#define ETHERTIA_WORLD_H

#include <unordered_map>
#include <glm/vec3.hpp>
#include <ethertia/world/chunk/Chunk.h>
#include <ethertia/world/gen/ChunkGenerator.h>
#include <ethertia/util/Tickable.h>
#include <ethertia/util/Log.h>
#include <ethertia/client/render/chunk/ChunkMeshGen.h>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#include "glm/gtx/string_cast.hpp"

class World
{
    std::unordered_map<glm::vec3, Chunk*> loadedChunks;

    ChunkGenerator chunkGenerator;

public:

    unsigned char getBlock(glm::vec3 blockpos) {
        Chunk* chunk = getLoadedChunk(blockpos);
        if (!chunk)
            return 0;
        return chunk->getBlock(blockpos);
    }

    Chunk* getLoadedChunk(glm::vec3 p) {
        return loadedChunks[Chunk::chunkpos(p)];
    }

    Chunk* provideChunk(glm::vec3 p) {
        Chunk* chunk = getLoadedChunk(p);
        if (chunk) return chunk;
        glm::vec3 chunkpos = Chunk::chunkpos(p);

        // chunk = chunkLoader.loadChunk();

        if (!chunk) {
            chunk = chunkGenerator.generateChunk(chunkpos);
        }

        loadedChunks[chunkpos] = chunk;

        // Log::info("ChunkLoaded " + glm::to_string(chunkpos));
        ChunkMeshGen::genMesh(chunk);
        return chunk;
    }

    void unloadChunk(glm::vec3 p)
    {
        auto itr = loadedChunks.find(Chunk::chunkpos(p));
        if (itr == loadedChunks.end())
            throw std::logic_error("Failed unload chunk. Not exists. "+glm::to_string(p));
        delete itr->second;
        loadedChunks.erase(itr);
    }

    std::unordered_map<glm::vec3, Chunk*> getLoadedChunks() {
        return loadedChunks;
    }


    // Tickable.
    void onTick() {

    }

    static void updateViewDistance(World* world, glm::vec3 p, int n)
    {
        glm::vec3 cpos = Chunk::chunkpos(p);

        // load chunks
        for (int dx = -n;dx <= n;dx++) {
            for (int dy = -n;dy <= n;dy++) {
                for (int dz = -n;dz <= n;dz++) {
                    world->provideChunk(cpos + glm::vec3(dx, dy, dz) * 16.0f);
                }
            }
        }

        // unload chunks
        int lim = n*Chunk::SIZE;
        std::vector<glm::vec3> unloads;  // separate iterate / remove
        for (auto itr : world->getLoadedChunks()) {
            glm::vec3 cp = itr.first;
            if (abs(cp.x-cpos.x) > lim || abs(cp.y-cpos.y) > lim || abs(cp.z-cpos.z) > lim) {
                unloads.push_back(cp);
            }
        }
        for (glm::vec3 cp : unloads) {
            world->unloadChunk(cp);
        }
    }
};

#endif //ETHERTIA_WORLD_H
