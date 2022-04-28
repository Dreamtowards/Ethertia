//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_WORLD_H
#define ETHERTIA_WORLD_H

#include <unordered_map>
#include <glm/vec3.hpp>
#include <ethertia/world/chunk/Chunk.h>
#include <ethertia/world/gen/ChunkGenerator.h>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#include "glm/gtx/string_cast.hpp"

class World
{
    std::unordered_map<glm::vec3, Chunk*> loadedChunks;

    ChunkGenerator chunkGenerator;

public:

    ubyte getBlock(glm::vec3 blockpos) {
        Chunk* chunk = getLoadedChunk(blockpos);
        if (!chunk) return 0;
        return chunk->getBlock(blockpos);
    }

    void setBlock(glm::vec3 blockpos, ubyte blockID) {
        Chunk* chunk = getLoadedChunk(blockpos);
        if (!chunk) return;
        chunk->setBlock(blockpos, blockID);
    }

    Chunk* provideChunk(glm::vec3 p);

    void unloadChunk(glm::vec3 p) {
        auto itr = loadedChunks.find(Chunk::chunkpos(p));
        if (itr == loadedChunks.end())
            throw std::logic_error("Failed unload chunk. Not exists. "+glm::to_string(p));
        delete itr->second;
        loadedChunks.erase(itr);
    }

    Chunk* getLoadedChunk(glm::vec3 p) {
        return loadedChunks[Chunk::chunkpos(p)];
    }

    std::unordered_map<glm::vec3, Chunk*> getLoadedChunks() {
        return loadedChunks;
    }

};

#endif //ETHERTIA_WORLD_H
