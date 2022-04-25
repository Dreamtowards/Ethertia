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

        Log::info("ChunkLoaded " + glm::to_string(chunkpos));
        ChunkMeshGen::genMesh(chunk);
        return chunk;
    }

    void unloadChunk(glm::vec3 p)
    {
        Log::info("Unload Chunk");
        auto itr = loadedChunks.find(Chunk::chunkpos(p));
        if (itr == loadedChunks.end())
            throw std::logic_error("Failed unload chunk. Not exists."+glm::to_string(p));
        delete itr->second;
        loadedChunks.erase(itr);
    }

    std::unordered_map<glm::vec3, Chunk*>* getLoadedChunks() {
        return &loadedChunks;
    }


    // Tickable.
    void onTick();
};

#endif //ETHERTIA_WORLD_H
